#include "../5/1202.h"
#include "../5/1202pp.h"
#include "grid.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include <cstdio>

#define FPS 32

typedef enum grid_kind {
  UNKNOWN,
  EMPTY,
  SCAFFOLD,
} grid_kind;

struct grid_type {
  grid_kind kind;
  size_t visited;

  grid_type() :
    kind(),
    visited(0) {}

  grid_type(grid_kind kind) :
    kind(kind),
    visited(0) {}

  bool operator==(const grid_kind& other) const {
    return kind == other;
  }

  bool operator!=(const grid_kind& other) const {
    return !(*this == other);
  }
};

// A linked list whose elements are stored on the stack.
// The list is singly linked and _backwards_. It must be
// traversed with a stack to reconstruct the proper order.
template <typename T>
class StackLinkedList {
private:
  T elem_;
  const StackLinkedList<T>* previous_;

public:
  StackLinkedList() :
    elem_(),
    previous_(nullptr) {}

  StackLinkedList(const StackLinkedList<T>* previous, T&& t) :
    previous_(previous),
    elem_(std::move(t)) {}

  StackLinkedList(const StackLinkedList<T>* previous) :
    previous_(previous),
    elem_() {}

  T& operator*() {
    return elem_;
  }

  T* operator->() {
    return &elem_;
  }

  // TODO: Make doubly linked, store pointer to head in each node, update the
  // previous nodes' forward pointer when constructing the next one, and update
  // the previous node's forward pointer when destructing the next one so that
  // we can do a single forward iteration.

  // Traverses the whole linked list and reconstructs the proper order.
  std::vector<const T*> get() {
    std::vector<const T*> reversed;
    for (const StackLinkedList<T>* current = this; current != nullptr; current = current->previous_) {
      reversed.push_back(&current->elem_);
    }

    std::vector<const T*> elems(reversed.size());
    for (size_t i = 0; i < reversed.size(); ++i) {
      elems[i] = reversed[reversed.size() - i - 1];
    }

    return elems;
  }
};

const std::vector<char> scaffold_chars = {
  '#', '<', '^', '>', 'v'
};

const std::vector<char> robot_chars = {
  '^', '>', 'v', '<'
};

const std::vector<Coord> directions = {
  Coord(0, -1),
  Coord(1, 0),
  Coord(0, 1),
  Coord(-1, 0),
};

bool is_scaffold_char(char c) {
  return std::find(scaffold_chars.cbegin(), scaffold_chars.cend(), c) != std::cend(scaffold_chars);
}

bool is_robot_char(char c) {
  return std::find(robot_chars.cbegin(), robot_chars.cend(), c) != std::cend(robot_chars);
}

Coord robot_char_to_dir(char c) {
  for (size_t i = 0; i < robot_chars.size(); ++i) {
    if (robot_chars[i] == c) {
      return directions[i];
    }
  }

  assert(false);
}

size_t dir_to_index(Coord c) {
  for (size_t i = 0; i < directions.size(); ++i) {
    if (c == directions[i]) {
      return i;
    }
  }
  assert(false);
}

struct Movement {
  // If true, contains a forward movement.
  // If false, contains a rotation.
  bool type;

  // If true, left.
  // If false, right.
  bool rotation;

  // The number of steps to move forward.
  size_t movement;

  explicit Movement(int n) :
    type(true),
    movement(n) {
      assert(n >= 0);
  }

  Movement() : Movement(0) {}

  explicit Movement(char c) :
    type(false) {
    if (c == 'L') {
      rotation = true;
    } else if (c == 'R') {
      rotation = false;
    } else {
      std::cerr << "Attempt to construct Movement with char '" << c << "'" << std::endl;
      exit(1);
    }
  }
};

std::ostream& operator<<(std::ostream& os, const Movement& x) {
  if (x.type) {
    os << x.movement;
  } else {
    if (x.rotation) {
      os << 'L';
    } else {
      os << 'R';
    }
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Movement>& x) {
  for (size_t i = 0; i < x.size(); ++i) {
    if (i != 0) {
      os << ",";
    }
    os << x[i];
  }
  return os;
}

std::vector<Movement> flatten_movement_list(const std::vector<const std::vector<Movement>*>& unflattened) {
  std::vector<Movement> m;
  for (const auto* step : unflattened) {
    m.insert(m.end(), step->begin(), step->end());
  }

  return m;
}

std::vector<Movement> compactify(const std::vector<Movement>& m) {
  std::vector<Movement> compactified;
  size_t in_offset = 0;
  while (in_offset < m.size()) {
    if (!m[in_offset].type) {
      // Rotation
      compactified.push_back(m[in_offset]);
      in_offset++;
    } else {
      // Forward movement.
      int total = 0;
      do {
        total += m[in_offset++].movement;
      } while (in_offset < m.size() && m[in_offset].type);
      compactified.push_back(Movement(total));
    }
  }
  return compactified;
}

std::vector<Movement> rotate(Coord heading_a, Coord heading_b) {
  assert(heading_a != heading_b);
  std::vector<Movement> m;
  int64_t dir_a = dir_to_index(heading_a);
  int64_t dir_b = dir_to_index(heading_b);

  int64_t difference = dir_b - dir_a;
  if (difference < 0) {
    difference += 4;
  }

  if (difference == 0) {
    assert(false);
  } else if (difference == 1) {
    m.push_back(Movement('R'));
  } else if (difference == 2) {
    m.push_back(Movement('R'));
    m.push_back(Movement('R'));
  } else if (difference == 3) {
    m.push_back(Movement('L'));
  } else {
    assert(false);
  }

  return m;

  // size_t positive_difference = (dir_b > dir_a) ? (dir_b - dir_a) : (directions.size() - dir_a + dir_b);
  // int centered_difference = (int)positive_difference - 2;
  // if (centered_difference < 0) {
  //   for (int i = 0; i < centered_difference * -1; ++i) {
  //     m.push_back(Movement('L'));
  //   }
  // } else if (centered_difference > 0) {
  //   for (int i = 0; i < centered_difference; ++i) {
  //     m.push_back(Movement('R'));
  //   }
  // } else {
  //   assert(false);
  // }

  return m;
}

void print_grid(const Grid<grid_type>& grid, Coord current, Coord current_heading) {
  size_t heading_index = dir_to_index(current_heading);
  char robot_char = robot_chars[heading_index];

  // TODO: Maybe provide iterator?
  for (int64_t y = grid.bounding_upper_left.y; y <= grid.bounding_bottom_right.y; ++y) {
    for (int64_t x = grid.bounding_upper_left.x; x <= grid.bounding_bottom_right.x; ++x) {
      char c;
      if (Coord(x, y) == current) {
        c = robot_char;
      } else if (grid.at(Coord(x, y)).kind == EMPTY) {
          c = ' ';
      } else if (grid.at(Coord(x, y)).kind == SCAFFOLD) {
        if (grid.at(Coord(x, y)).visited > 0) {
          c = '.';
        } else {
          c = '#';
        }
      } else {
          c = '?';
      }
      std::cout << c;
    }
    std::cout << std::endl;
  }
}

std::vector<Coord> get_intersections(Grid<grid_type>& grid) {
  std::vector<Coord> intersections;
  for (int64_t y = grid.bounding_upper_left.y; y <= grid.bounding_bottom_right.y; ++y) {
    for (int64_t x = grid.bounding_upper_left.x; x <= grid.bounding_bottom_right.x; ++x) {
      Coord current(x, y);
      bool has_adjacent_empty = false;
      for (const auto& dir : directions) {
        Coord neighbor = current + dir;
        if (grid.at(neighbor) == EMPTY) {
          has_adjacent_empty = true;
        }
      }
      if (!has_adjacent_empty) {
        intersections.push_back(current);
      }
    }
  }

  return intersections;
}

int64_t aggregate_intersections(const std::vector<Coord>& intersections) {
  int64_t sum = 0;
  for (const auto& intersection : intersections) {
    sum += intersection.x * intersection.y;
  }
  return sum;
}

void print_frame(Process& process) {
  char prev = '0';
  int64_t y = 0;
  int64_t x = 0;
  for (int64_t val : process) {
    char cur = (char)val;
    std::cout << cur;
    if (prev == '\n' && cur == '\n') {
      break;
    }
    prev = cur;
  }
}

class Scene {
private:
  Grid<grid_type> grid_;
  std::vector<Coord> intersections_;
  Coord robot_location_;
  Coord robot_heading_;

  void read_frame(Process *process);

  bool visited(Coord coord) const {
    return grid_.at(coord).visited != 0;
  }

  void visit(Coord coord) {
    grid_[coord].visited++;
  }

  void visit(Coord coord, size_t* visited_count) {
    if (visited(coord)) {
      *visited_count += 1;
    }
    grid_[coord].visited++;
  }

  void unvisit(Coord coord) {
    grid_[coord].visited--;
  }

  std::vector<Coord> get_next_coord_candidates(Coord current, Coord previous) const {
    std::vector<Coord> candidates;
    for (Coord dir : directions) {
      Coord candidate_coord = current + dir;
      grid_type candidate = grid_.at(candidate_coord);
      // Intersections may be crossed multiple times.
      if ((!candidate.visited && candidate.kind == SCAFFOLD) || (is_intersection(candidate_coord) && candidate_coord != previous)) {
        candidates.push_back(candidate_coord);
      }
    }

    // Coord target(15, 34);
    // if (current == target) {
    //   std::cout << "Neighbors of " << target << ": " << std::endl;
    //   for (const auto& candidate : candidates) {
    //     std::cout << "  " << candidate << std::endl;
    //   }
    // }

    return candidates;
  }

  bool is_intersection(Coord coord) const {
    return std::find(intersections_.cbegin(), intersections_.cend(), coord) != intersections_.cend();
  }

  // Given a current heading, a current location, and a desired location,
  // returns a series of movements to go to that location. Also returns
  // the heading once moved to the new location.
  void move(Coord from, Coord heading, Coord next, Coord *new_heading, std::vector<Movement>* m) {
    Coord direction = next - from;

    *new_heading = direction;

    if (direction != heading) {
      std::vector<Movement> rotations;
      rotations = rotate(heading, direction);
      m->insert(m->end(), rotations.begin(), rotations.end());
    }

    assert(llabs(direction.x) + llabs(direction.y) == 1);
    m->push_back(Movement(1));
  }

  size_t get_total_scaffold() const {
    size_t total = 0;
    for (int64_t y = grid_.bounding_upper_left.y; y <= grid_.bounding_bottom_right.y; ++y) {
      for (int64_t x = grid_.bounding_upper_left.x; x <= grid_.bounding_bottom_right.x; ++x) {
        if (grid_.at(Coord(x, y)) == SCAFFOLD) {
          total++;
        }
      }
    }

    return total;
  }

public:
  Scene(Process *process) :
    grid_(),
    intersections_(),
    robot_location_(),
    robot_heading_()
  {
    read_frame(process);
    intersections_ = get_intersections(grid_);
  }

  // TODO: Figure out interface later.
  void plan();

  void plan(Coord current, Coord current_heading, Coord previous, const StackLinkedList<std::vector<Movement>>* previous_movements, size_t visited_count, size_t total_scaffold, std::vector<std::vector<Movement>>* possible_paths);

  void print(Coord current, Coord current_heading) const {
    print_grid(grid_, current, current_heading);
  }

  void print() const {
    print(robot_location_, robot_heading_);
  }
};

void Scene::plan(Coord current, Coord current_heading, Coord previous, const StackLinkedList<std::vector<Movement>>* previous_movements, size_t visited_count, size_t total_scaffold, std::vector<std::vector<Movement>>* possible_paths) {
  StackLinkedList<std::vector<Movement>> m(previous_movements);

  visit(current, &visited_count);

  // TODO: If going to the next node would create unreachable points, abort this branch.
  // Only do this check when we branch, as this will be expensive.

  assert(grid_.at(current).kind == SCAFFOLD);

  print(current, current_heading);
  usleep(1000000 / FPS);

  std::vector<Coord> next_candidates = get_next_coord_candidates(current, previous);
  if (next_candidates.size() == 0) {
    // We've reached the end.
    if (visited_count == total_scaffold) {
      // We've visited all scaffolding tiles. Output path.
      std::vector<Movement> path = compactify(flatten_movement_list(m.get()));

      std::cout << "Path: " << path << std::endl;
      possible_paths->push_back(path);
    }
    // If we're at a dead end. Output nothing.
  } else {
    for (Coord next : next_candidates) {
      Coord new_heading;
      move(current, current_heading, next, &new_heading, &(*m));
      plan(next, new_heading, current, &m, visited_count, total_scaffold, possible_paths);
    }
  }

  // TODO: Is it okay to unvisit intersections?
  unvisit(current);
}

void Scene::plan() {
  // TODO: Incrementally output rather than storing them all in memory.
  std::vector<std::vector<Movement>> possible_paths;
  plan(robot_location_, robot_heading_, robot_location_, nullptr, 0, get_total_scaffold(), &possible_paths);

  for (const auto& possible_path : possible_paths) {
    std::cout << possible_path << std::endl;
  }
}

void Scene::read_frame(Process *process) {
  char prev = '0';
  int64_t y = 0;
  int64_t x = 0;
  bool found_robot = false;
  for (int64_t val : *process) {
    char cur = (char)val;
    if (cur == '\n') {
      x = -1;
      y += 1;
    } else {
      if (is_scaffold_char(cur)) {
        grid_[Coord(x, y)] = SCAFFOLD;
      } else {
        grid_[Coord(x, y)] = EMPTY;
      }

      if (is_robot_char(cur)) {
        robot_location_ = Coord(x, y);
        robot_heading_ = robot_char_to_dir(cur);
        found_robot = true;
      }
    }
    if (prev == '\n' && cur == '\n') {
      break;
    }
    prev = cur;
    x += 1;
  }

  if (!found_robot) {
    std::cerr << "Failed to find robot while parsing scene." << std::endl;
    exit(1);
  }
}



int main(int argc, char **argv) {
  Program program("puzzle.1202");
  Process process(program);

  // Enables interactive mode
  process.set_address(0, 2);


  // Reads the first frame and parses it.
  Scene scene(&process);

  std::vector<Movement> m = {
    Movement('L'),
    Movement(10),
    Movement('R'),
    Movement(5),
  };

  // TODO: Intelligently calculate the plan.

  scene.print();

  scene.plan();

  // // Main movement routine.
  // process.write("A,B,B\n");

  // // Movement function A.
  // process.write("R,8\n");

  // // Movement function B.
  // process.write("R,10\n");

  // // Movement function C.
  // process.write("L,1\n");

  // // Enable Continuous video
  // process.write("y\n");

  // // Required to get the process to read the input.
  // process.exec();

  // while (true) {
  //   print_frame(process);
  //   usleep(1000000 / 2);
  // }

  // // TODO: Collect the integer emitted at the end.

  return 0;
}
