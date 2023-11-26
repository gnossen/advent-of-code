#include "../5/1202.h"
#include "../5/1202pp.h"
#include "grid.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <cstdio>

typedef enum grid_type {
  UNKNOWN,
  EMPTY,
  SCAFFOLD,
} grid_type;

const std::vector<char> scaffold_chars = {
  '#', '<', '^', '>', 'v'
};

const std::vector<Coord> directions = {
  Coord(0, 1),
  Coord(0, -1),
  Coord(1, 0),
  Coord(-1, 0),
};

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

int main(int argc, char **argv) {
  Program program("puzzle.1202");
  Process process(program);

  Grid<grid_type> grid;

  char prev = '0';
  int64_t y = 0;
  int64_t x = 0;
  for (int64_t val : process) {
    char cur = (char)val;
    if (cur == '\n') {
      x = -1;
      y += 1;
    } else {
      if (std::find(scaffold_chars.cbegin(), scaffold_chars.cend(), cur) != std::cend(scaffold_chars)) {
        grid[Coord(x, y)] = SCAFFOLD;
      } else {
        grid[Coord(x, y)] = EMPTY;
      }
    }
    // std::cout << cur;
    if (prev == '\n' && cur == '\n') {
      break;
    }
    prev = cur;
    x += 1;
  }

  print_grid(grid);

  auto intersections = get_intersections(grid);
  for (auto intersection : intersections) {
    std::cout << intersection << std::endl;
  }

  std::cout << aggregate_intersections(intersections) << std::endl;

  return 0;
}
