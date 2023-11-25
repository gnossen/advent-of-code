#include "../dynamic_grid.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

void test_basic() {
  dynamic_grid_t *grid = create_dynamic_grid();
  coord_t to_add[] = {
    {20, 20},
    {-20, 20},
    {1000, 0},
  };
  coord_t to_check[] = {
    {0, 0},
    {-19, 19},
    {0, 1000},
  };
  for (size_t i = 0; i < sizeof(to_check) / sizeof(coord_t); ++i) {
    assert(get_point(grid, to_check[i]) == UNKNOWN);
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(coord_t); ++i) {
    assert(get_point(grid, to_add[i]) == UNKNOWN);
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(coord_t); ++i) {
    set_point(grid, to_add[i], WALL);
  }
  for (size_t i = 0; i < sizeof(to_add) / sizeof(coord_t); ++i) {
    assert(get_point(grid, to_add[i]) == WALL);
  }
  for (size_t i = 0; i < sizeof(to_check) / sizeof(coord_t); ++i) {
    assert(get_point(grid, to_check[i]) == UNKNOWN);
  }
  assert(grid->bounding_upper_left.x == -20);
  assert(grid->bounding_upper_left.y == 0);
  assert(grid->bounding_bottom_right.x == 1000);
  assert(grid->bounding_bottom_right.y == 20);
  destroy_dynamic_grid(grid);
}

void test_failure() {
  dynamic_grid_t *grid = create_dynamic_grid();
  set_point(grid, make_coord(0, 1), 2);
  set_point(grid, make_coord(0, 2), 2);
  set_point(grid, make_coord(0, 3), 2);
  set_point(grid, make_coord(0, 4), 2);
  set_point(grid, make_coord(0, 5), 1);
  set_point(grid, make_coord(-1, 4), 1);
  set_point(grid, make_coord(1, 4), 2);
  set_point(grid, make_coord(1, 5), 1);
  set_point(grid, make_coord(1, 3), 1);
  set_point(grid, make_coord(2, 4), 2);
  set_point(grid, make_coord(2, 5), 2);
  set_point(grid, make_coord(2, 6), 2);
  set_point(grid, make_coord(2, 7), 1);
  set_point(grid, make_coord(1, 6), 1);
  set_point(grid, make_coord(3, 6), 2);
  set_point(grid, make_coord(3, 7), 1);
  set_point(grid, make_coord(3, 5), 1);
  set_point(grid, make_coord(4, 6), 2);
  set_point(grid, make_coord(4, 7), 1);
  set_point(grid, make_coord(4, 5), 2);
  set_point(grid, make_coord(4, 4), 2);
  set_point(grid, make_coord(4, 3), 2);
  set_point(grid, make_coord(4, 2), 2);
  set_point(grid, make_coord(0, 1), 2);
  set_point(grid, make_coord(0, 2), 2);
  set_point(grid, make_coord(0, 3), 2);
  set_point(grid, make_coord(0, 4), 2);
  set_point(grid, make_coord(0, 5), 1);
  set_point(grid, make_coord(-1, 4), 1);
  set_point(grid, make_coord(1, 4), 2);
  set_point(grid, make_coord(1, 5), 1);
  set_point(grid, make_coord(1, 3), 1);
  set_point(grid, make_coord(2, 4), 2);
  set_point(grid, make_coord(2, 5), 2);
  set_point(grid, make_coord(2, 6), 2);
  set_point(grid, make_coord(2, 7), 1);
  set_point(grid, make_coord(1, 6), 1);
  set_point(grid, make_coord(3, 6), 2);
  set_point(grid, make_coord(3, 7), 1);
  set_point(grid, make_coord(3, 5), 1);
  set_point(grid, make_coord(4, 6), 2);
  set_point(grid, make_coord(4, 7), 1);
  set_point(grid, make_coord(4, 5), 2);
  set_point(grid, make_coord(4, 4), 2);
  set_point(grid, make_coord(4, 3), 2);
  set_point(grid, make_coord(4, 2), 2);
  set_point(grid, make_coord(4, 1), 1);
  set_point(grid, make_coord(3, 2), 1);
  set_point(grid, make_coord(5, 2), 2);
  set_point(grid, make_coord(5, 3), 1);
  set_point(grid, make_coord(5, 1), 1);
  set_point(grid, make_coord(6, 2), 2);
  set_point(grid, make_coord(6, 3), 1);
  set_point(grid, make_coord(6, 1), 1);
  set_point(grid, make_coord(7, 2), 2);
  set_point(grid, make_coord(7, 3), 1);
  set_point(grid, make_coord(7, 1), 1);
  set_point(grid, make_coord(8, 2), 2);
  assert(get_point(grid, make_coord(8, 2)) == 2);
  destroy_dynamic_grid(grid);
}

int main(int argc, char **argv) {
  test_basic();
  test_failure();
  printf("SUCCESS\n");
  return 0;
}
