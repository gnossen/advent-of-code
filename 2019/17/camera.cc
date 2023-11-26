#include "../5/1202.h"
#include "../5/1202pp.h"
#include "grid.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include <cstdio>

typedef enum grid_type {
  UNKNOWN,
  EMPTY,
  SCAFFOLD,
} grid_type;

const std::vector<char> scaffold_chars = {
  '#', '<', '^', '>', 'v'
};

const std::vector<char> robot_chars = {
  '<', '^', '>', 'v'
};

const std::vector<Coord> directions = {
  Coord(-1, 0),
  Coord(0, -1),
  Coord(0, 1),
  Coord(1, 0),
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

void print_grid(const Grid<grid_type>& grid) {
  // TODO: Maybe provide iterator?
  for (int64_t y = grid.bounding_upper_left.y; y <= grid.bounding_bottom_right.y; ++y) {
    for (int64_t x = grid.bounding_upper_left.x; x <= grid.bounding_bottom_right.x; ++x) {
      char c;
      switch (grid[Coord(x, y)]) {
        case EMPTY:
          c = ' ';
          break;
        case SCAFFOLD:
          c = '#';
          break;
        default:
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
};

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

  // TODO: Intelligently calculate the plan.

  // Main movement routine.
  process.write("A,B,B\n");

  // Movement function A.
  process.write("R,8\n");

  // Movement function B.
  process.write("R,10\n");

  // Movement function C.
  process.write("L,1\n");

  // Enable Continuous video
  process.write("y\n");

  // Required to get the process to read the input.
  process.exec();

  while (true) {
    print_frame(process);
    usleep(1000000 / 2);
  }

  // TODO: Collect the integer emitted at the end.

  return 0;
}
