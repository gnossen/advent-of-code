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
  destroy_dynamic_grid(grid);
}

int main(int argc, char **argv) {
  test_basic();
  printf("SUCCESS\n");
  return 0;
}
