#include <inttypes.h>
#include <stdbool.h>

typedef struct coord_t {
  int64_t x;
  int64_t y;
} coord_t;

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

  // TODO: Add back bounding box.

  grid_status *data;
} dynamic_grid_t;

// TODO: Pump these up after testing.
#define INITIAL_WIDTH 16
#define INITIAL_HEIGHT 16

dynamic_grid_t *create_dynamic_grid();
void destroy_dynamic_grid(dynamic_grid_t *grid);
void set_point(dynamic_grid_t *grid, coord_t coord, grid_status status);
grid_status get_point(dynamic_grid_t *grid, coord_t coord);
