#include "../5/1202.h"
#include "dynamic_grid.h"
#include "stack.h"

#include <stdlib.h>
#include <assert.h>

#define DIRECTION_COUNT 4
#define NO_DIRECTION 5

#define HIT_WALL            0
#define MOVED_TO_EMPTY      1
#define ENCOUNTERED_TARGET  2

coord_t directions[] = {
  {0, 1},
  {0, -1},
  {-1, 0},
  {1, 0},
};

int64_t lookup_direction(coord_t direction) {
  for (int64_t i = 0; i < 4; ++i) {
    if (direction.x == directions[i].x && direction.y == directions[i].y) {
      return i + 1;
    }
  }

  fprintf(stderr, "Attempt to move in direction (%jd, %jd)\n", direction.x, direction.y);
  exit(1);
}

grid_status go_in_direction(size_t dir_index, process_t *process) {
  buffer_write(process->input, (int64_t)dir_index + 1);
  // printf("Direction: %d\n", dir_index + 1);
  process_status status = execute(process);

  process_status expected_status = AWAITING_READ;
  if (status != expected_status) {
    fprintf(stderr, "Expected process status %d but got %d\n", expected_status, status);
    exit(1);
  }
  int64_t grid_status = buffer_read(process->output);
  // printf("Response: %d\n", grid_status);

  // Need to add one to make it compatible with our enum which has UNKNOWN at 0
  return grid_status + 1;
}

size_t reverse_dir(size_t dir) {
  switch (dir) {
    case 0:
      return 1;
      break;
    case 1:
      return 0;
      break;
    case 2:
      return 3;
      break;
    case 3:
      return 2;
      break;
    default:
      fprintf(stderr, "Encountered unknown direction %d\n", dir);
      exit(1);
  }
}

void backtrack(process_t *process, size_t direction_index) {
  if (direction_index == NO_DIRECTION) { return; }
  grid_status status = go_in_direction(reverse_dir(direction_index), process);
  assert(status == EMPTY);
}

void print_grid(dynamic_grid_t *grid, coord_t current) {
  for (int64_t y = grid->bounding_upper_left.y; y <= grid->bounding_bottom_right.y; ++y) {
    for (int64_t x = grid->bounding_upper_left.x; x <= grid->bounding_bottom_right.x; ++x) {
      coord_t coord = {x, y};
      grid_status status = get_point(grid, coord);
      if (current.x == x && current.y == y) {
        printf("D");
      } else if (status == UNKNOWN) {
        printf(" ");
      } else if (status == EMPTY) {
        printf(".");
      } else if (status == WALL) {
        printf("#");
      } else if (status == TARGET) {
        printf("T");
      }
    }
    printf("\n");
  }

  printf("\n");
}


// TODO: Do this with a manually-managed stack.

/* Returns distance from target if this path contains the target, otherwise
 * returns -1.
 *
 * direction_index - the index of the direction used to get to this tile from
 *   the previous. The reverse direction is used in backtracking.
 */
int64_t visit_node(
    coord_t coord,
    size_t distance_from_origin,
    dynamic_grid_t *grid,
    process_t *process,
    size_t direction_index)
{
  // printf("Visiting (%jd, %jd)\n", coord.x, coord.y);

  // At this point, the robot is at coord and the current coord has already been marked.
  if (get_point(grid, coord) == TARGET) {
    backtrack(process, direction_index);
    return distance_from_origin;
  }

  size_t min_dist = SIZE_MAX;
  for (size_t i = 0; i < DIRECTION_COUNT; ++i) {
    coord_t candidate = coord_add(coord, directions[i]);
    // Skip over anything that's already been visited.
    if (get_point(grid, candidate) == UNKNOWN) {
      grid_status status = go_in_direction(i, process);
      set_point(grid, candidate, status);

      print_grid(grid, coord);
      usleep(31250);

      if (status != WALL) {
        // printf("  Visiting non-wall at (%jd, %jd)\n", candidate.x, candidate.y);
        int64_t dist = visit_node(candidate, distance_from_origin + 1, grid, process, i);
        // printf("Returning to (%jd, %jd)\n", coord.x, coord.y);
        if (dist != -1 && dist < min_dist) {
          min_dist = dist;
        }
      } else {
        // printf("  Skipping wall at (%jd, %jd)\n", candidate.x, candidate.y);
      }
      // If wall, we just skip over it -- current position hasn't updated.
    } else {
      // printf("Skipping previously visited direction %d\n", i + 1);
    }
  }

  backtrack(process, direction_index);

  if (min_dist != SIZE_MAX) { return min_dist; }

  return -1;
}

int main(int argc, int **argv) {
  program_t program = program_from_text_filepath("puzzle.1202");
  process_t *process = instantiate_process(program);

  // stack_t *stack = create_stack();
  dynamic_grid_t *grid = create_dynamic_grid();

  coord_t origin = {0, 0};
  int64_t dist = visit_node(origin, 0, grid, process, NO_DIRECTION);
  printf("Distance: %d\n", dist);

  // stack_push(stack, current);

  // while (1) {
  //   // At this point the droid is located at current
  //   // and there is a corresponding stack entry for it at the top.
  //   if (get_point(grid, current) == UNKNOWN) {
  //     set_point(grid, current, EMPTY);
  //     for (size_t i = 0; i < 4; ++i) {
  //       coord_t candidate = coord_add(current, directions[i]);
  //       if (get_point(candidate) == UNKNOWN) {
  //         stack_push(stack, candidate);
  //       }
  //     }
  //   } else {
  //     stack_pop(grid);
  //   }

  //   // At this point, we should be considering the top element of
  //   // the stack, which is guaranteed to be only one tile away.
  //   while (1) {
  //     coord_t candidate = stack_peek(stack);
  //     coord_t direction = coord_sub(candidate, current);
  //     int64_t dir_code = lookup_direction(direction);
  //     buffer_write(process->input, dir_code);
  //     process_status status = execute(process);
  //     assert(status == AWAITING_WRITE);
  //     int64_t resp = buffer_read(process->output);
  //     if (resp == 0) {
  //       set_point(candidate, WALL);
  //       stack_pop(stack);
  //     } else if (resp == 1) {
  //       current = candidate;
  //       break;
  //     } else {
  //       // TODO: Print screen.
  //     }
  //   }
  // }

  destroy_dynamic_grid(grid);
  // destroy_stack(stack);
  destroy_process(process);
  destroy_program(program);
  return 0;
}
