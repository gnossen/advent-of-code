#include "../grid.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <vector>

typedef enum grid_status {
  UNKNOWN,
  WALL,
  EMPTY,
  TARGET,
  OXYGEN,
} grid_status;

void test_basic() {
  Grid<grid_status> grid;
  Coord to_add[] = {
    {20, 20},
    {-20, 20},
    {1000, 0},
  };
  Coord to_check[] = {
    {0, 0},
    {-19, 19},
    {0, 1000},
  };
  for (size_t i = 0; i < sizeof(to_check) / sizeof(Coord); ++i) {
    assert(grid.at(to_check[i]) == UNKNOWN);
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(Coord); ++i) {
    assert(grid.at(to_add[i]) == UNKNOWN);
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(Coord); ++i) {
    grid[to_add[i]] = WALL;
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(Coord); ++i) {
    assert(grid.at(to_add[i]) == WALL);
  }
  for (size_t i = 0; i < sizeof(to_check) / sizeof(Coord); ++i) {
    assert(grid.at(to_check[i]) == UNKNOWN);
  }
  assert(grid.bounding_upper_left.x == -20);
  assert(grid.bounding_upper_left.y == 0);
  assert(grid.bounding_bottom_right.x == 1000);
  assert(grid.bounding_bottom_right.y == 20);
}

void test_failure() {
  Grid<size_t> grid;
  std::vector<std::pair<Coord, size_t>> to_add {
    {{0, 1}, 2},
    {{0, 2}, 2},
    {{0, 3}, 2},
    {{0, 4}, 2},
    {{0, 5}, 1},
    {{-1, 4}, 1},
    {{1, 4}, 2},
    {{1, 5}, 1},
    {{1, 3}, 1},
    {{2, 4}, 2},
    {{2, 5}, 2},
    {{2, 6}, 2},
    {{2, 7}, 1},
    {{1, 6}, 1},
    {{3, 6}, 2},
    {{3, 7}, 1},
    {{3, 5}, 1},
    {{4, 6}, 2},
    {{4, 7}, 1},
    {{4, 5}, 2},
    {{4, 4}, 2},
    {{4, 3}, 2},
    {{4, 2}, 2},
    {{0, 1}, 2},
    {{0, 2}, 2},
    {{0, 3}, 2},
    {{0, 4}, 2},
    {{0, 5}, 1},
    {{-1, 4}, 1},
    {{1, 4}, 2},
    {{1, 5}, 1},
    {{1, 3}, 1},
    {{2, 4}, 2},
    {{2, 5}, 2},
    {{2, 6}, 2},
    {{2, 7}, 1},
    {{1, 6}, 1},
    {{3, 6}, 2},
    {{3, 7}, 1},
    {{3, 5}, 1},
    {{4, 6}, 2},
    {{4, 7}, 1},
    {{4, 5}, 2},
    {{4, 4}, 2},
    {{4, 3}, 2},
    {{4, 2}, 2},
    {{4, 1}, 1},
    {{3, 2}, 1},
    {{5, 2}, 2},
    {{5, 3}, 1},
    {{5, 1}, 1},
    {{6, 2}, 2},
    {{6, 3}, 1},
    {{6, 1}, 1},
    {{7, 2}, 2},
    {{7, 3}, 1},
    {{7, 1}, 1},
    {{8, 2}, 2},
  };

  for (const auto& p : to_add) {
    grid[p.first] = p.second;
  }
  assert(grid.at(Coord(8, 2)) == 2);
}

int main(int argc, char **argv) {
  test_basic();
  test_failure();
  printf("SUCCESS\n");
  return 0;
}
