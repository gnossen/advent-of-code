#include "dynamic_grid.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// TODO: Remove.
#include <stdio.h>

coord_t make_coord(int64_t x, int64_t y) {
  coord_t z = {
    .x = x,
    .y = y,
  };
  return z;
}

coord_t coord_add(coord_t a, coord_t b) {
  coord_t c = {
    .x = a.x + b.x,
    .y = a.y + b.y,
  };

  return c;
}

coord_t coord_sub(coord_t a, coord_t b) {
  coord_t c = {
    .x = a.x - b.x,
    .y = a.y - b.y,
  };

  return c;
}

dynamic_grid_t *create_dynamic_grid() {
  dynamic_grid_t *grid = malloc(sizeof(dynamic_grid_t));
  grid->capacity.x = INITIAL_WIDTH;
  grid->capacity.y = INITIAL_HEIGHT;

  grid->data = malloc(sizeof(grid_status) * grid->capacity.x * grid->capacity.y);
  memset(grid->data, 0, sizeof(grid_status) * grid->capacity.x * grid->capacity.y);

  grid->has_points = false;
}

void destroy_dynamic_grid(dynamic_grid_t *grid) {
  free(grid->data);
  free(grid);
}

// Determines the multiplicative scale by which the buffer must grow to
// accommodate the new point.
static size_t determine_scale(int64_t capacity, int64_t requirement) {
  int64_t directional_capacity = capacity / 2;
  int64_t difference = llabs(directional_capacity - requirement);
  size_t multiplier = 1;
  while (directional_capacity <= difference) {
    directional_capacity *= 2;
    multiplier *= 2;
  }
  return multiplier;
}

/* coord must be in the native coordinate system of the buffer, i.e. no translation needed.
 */
static void grow_capacity(dynamic_grid_t *grid, coord_t coord) {
  size_t x_scale = determine_scale(grid->capacity.x, coord.x);
  size_t y_scale = determine_scale(grid->capacity.x, coord.y);
  size_t scale = (x_scale > y_scale) ? x_scale : y_scale;
  assert(scale > 1);
  // printf("Growing capacity by scale %d.\n", scale);

  coord_t new_capacity = {
    .x = grid->capacity.x * scale,
    .y = grid->capacity.y * scale,
  };

  grid_status *new_data = malloc(sizeof(grid_status) * new_capacity.x * new_capacity.y);

  // Set everything to unknown
  memset(new_data, 0, sizeof(grid_status) * new_capacity.x * new_capacity.y);


  // Translation from the top left of the new buffer to top-left of the old one.
  coord_t delta_translation = {
    .x = (new_capacity.x / 2) - (grid->capacity.x / 2),
    .y = (new_capacity.y / 2) - (grid->capacity.y / 2)
  };

  // Overlay the existing data on top.
  for (size_t y = 0; y < grid->capacity.y; ++y) {
    for (size_t x = 0; x < grid->capacity.x; ++x) {
      size_t translated_x = x + delta_translation.x;
      size_t translated_y = y + delta_translation.y;
      if (grid->data[y * grid->capacity.x + x] != UNKNOWN) {
      }
      new_data[translated_y * new_capacity.x + translated_x] = grid->data[y * grid->capacity.x + x];
    }
  }

  grid_status *old_data = grid->data;
  grid->data = new_data;

  grid->translation.x += delta_translation.x;
  grid->translation.y += delta_translation.y;

  grid->capacity = new_capacity;

  free(old_data);
}

static coord_t to_grid_native(dynamic_grid_t *grid, coord_t coord) {
  coord_t adjusted_coord = {
    .x = grid->translation.x + coord.x,
    .y = grid->translation.y + coord.y
  };
  return adjusted_coord;
}

/* coord is assumed to be relative to the top left of the capacity, i.e no
 * translation.
 */
static bool out_of_bounds(dynamic_grid_t *grid, coord_t coord) {
  if (coord.x < 0 || coord.x >= grid->capacity.x ||
      coord.y < 0 || coord.y >= grid->capacity.y) {
    return true;
  }
  return false;
}


void set_point(dynamic_grid_t *grid, coord_t coord, grid_status status) {
  // printf("grid_trace: set_point((%jd, %jd), %d)\n", coord.x, coord.y, status);
  if (!grid->has_points) {
    // Put the point at the center of the allocated grid.
    grid->has_points = true;

    grid->translation.x = (grid->capacity.x / 2) - coord.x;
    grid->translation.y = (grid->capacity.y / 2) - coord.y;

    grid->bounding_upper_left = coord;
    grid->bounding_bottom_right = coord;
  }

  if (coord.x < grid->bounding_upper_left.x) { grid->bounding_upper_left.x = coord.x; }
  if (coord.y < grid->bounding_upper_left.y) { grid->bounding_upper_left.y = coord.y; }
  if (coord.x > grid->bounding_bottom_right.x) { grid->bounding_bottom_right.x = coord.x; }
  if (coord.y > grid->bounding_bottom_right.y) { grid->bounding_bottom_right.y = coord.y; }

  coord_t adjusted_coord = to_grid_native(grid, coord);

  if (out_of_bounds(grid, adjusted_coord)) {
    grow_capacity(grid, adjusted_coord);
    adjusted_coord = to_grid_native(grid, coord);
  }

  // printf("  stored (%jd, %jd) at (%jd, %jd)\n", coord.x, coord.y, adjusted_coord.x, adjusted_coord.y);
  grid->data[adjusted_coord.y * grid->capacity.x + adjusted_coord.x] = status;
}

grid_status get_point(dynamic_grid_t *grid, coord_t coord) {
  coord_t adjusted_coord = to_grid_native(grid, coord);

  // printf("  retrieving (%jd, %jd) from (%jd, %jd)\n", coord.x, coord.y, adjusted_coord.x, adjusted_coord.y);
  if (out_of_bounds(grid, adjusted_coord)) {
    // printf("grid_trace: get_point((%jd, %jd)) = 0 (out of bounds)\n", coord.x, coord.y);
    return UNKNOWN;
  }

  grid_status val = grid->data[adjusted_coord.y * grid->capacity.x + adjusted_coord.x];
  // printf("grid_trace: get_point((%jd, %jd)) = %d\n", coord.x, coord.y, val);
  return val;
}
