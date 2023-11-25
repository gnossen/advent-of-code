#ifndef _DYNAMIC_GRID_H
#define _DYNAMIC_GRID_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct coord_t {
  int64_t x;
  int64_t y;
} coord_t;

coord_t make_coord(int64_t x, int64_t y);
coord_t coord_add(coord_t a, coord_t b);
coord_t coord_sub(coord_t a, coord_t b);

typedef enum grid_status {
  UNKNOWN,
  WALL,
  EMPTY,
  TARGET
} grid_status;

typedef struct dynamic_grid_t {
  coord_t translation;

  // The width and height of the allocated buffer;
  coord_t capacity;

  // Initially set to false. While there are no populated points, translation
  // is unset.
  bool has_points;

  coord_t bounding_upper_left;
  coord_t bounding_bottom_right;

  grid_status *data;
} dynamic_grid_t;

// TODO: Pump these up after testing.
#define INITIAL_WIDTH 16
#define INITIAL_HEIGHT 16

dynamic_grid_t *create_dynamic_grid();
void destroy_dynamic_grid(dynamic_grid_t *grid);
void set_point(dynamic_grid_t *grid, coord_t coord, grid_status status);
grid_status get_point(dynamic_grid_t *grid, coord_t coord);

#endif //  _DYNAMIC_GRID_H
