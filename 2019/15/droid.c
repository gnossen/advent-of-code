#include "../5/1202.h"
#include "dynamic_grid.h"
#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

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

size_t FPS = 32;

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
      } else if (status == OXYGEN) {
        printf("O");
      } else {
        printf("?");
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
 * target - Out param. The location of the target.
 */
int64_t visit_node(
    coord_t coord,
    size_t distance_from_origin,
    dynamic_grid_t *grid,
    process_t *process,
    size_t direction_index,
    coord_t *target,
    bool draw)
{
  // printf("Visiting (%jd, %jd)\n", coord.x, coord.y);

  // At this point, the robot is at coord and the current coord has already been marked.
  if (get_point(grid, coord) == TARGET) {
    backtrack(process, direction_index);
    *target = coord;
    return distance_from_origin;
  }

  size_t min_dist = SIZE_MAX;
  for (size_t i = 0; i < DIRECTION_COUNT; ++i) {
    coord_t candidate = coord_add(coord, directions[i]);
    // Skip over anything that's already been visited.
    if (get_point(grid, candidate) == UNKNOWN) {
      grid_status status = go_in_direction(i, process);
      set_point(grid, candidate, status);

      if (draw) {
        print_grid(grid, coord);
        usleep(1000000 / FPS);
      }

      if (status != WALL) {
        // printf("  Visiting non-wall at (%jd, %jd)\n", candidate.x, candidate.y);
        int64_t dist = visit_node(candidate, distance_from_origin + 1, grid, process, i, target, draw);
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

/* Returns true if oxygen has not filled the entire room, false otherwise.
 * 
 * stack contains the set of tiles that will gain oxygen on this tick.
 *
 * After the function has run, stack contains the next set of candidates.
 */
bool run_oxygen_tick(dynamic_grid_t *grid, stack_t *stack) {
  stack_t *next_candidates = create_stack();

  while (!stack_empty(stack)) {
    coord_t next = stack_pop(stack);
    set_point(grid, next, OXYGEN);
    for (size_t i = 0; i < DIRECTION_COUNT; ++i) {
      coord_t candidate = coord_add(next, directions[i]);
      if (get_point(grid, candidate) == EMPTY) {
        stack_push(next_candidates, candidate);
      }
    }
  }

  // Copy tmp array into outparam.
  while (!stack_empty(next_candidates)) {
    stack_push(stack, stack_pop(next_candidates));
  }

  destroy_stack(next_candidates);

  return !stack_empty(stack);
}

/* Returns the number of minutes to fully fill the room with oxygen.
 *
 */
size_t fill_with_oxygen(dynamic_grid_t *grid, coord_t target) {
  size_t minutes = 0;
  stack_t *stack = create_stack();
  stack_push(stack, target);
  while (run_oxygen_tick(grid, stack)) {
    print_grid(grid, target);
    usleep(1000000 / FPS);
    minutes += 1;
  }
  destroy_stack(stack);

  return minutes;
}

int main(int argc, int **argv) {
  char *fps_str = getenv("FPS");
  if (fps_str != NULL && strlen(fps_str) > 0) {
    // TODO: Parse error checking.
    FPS = atoi(fps_str);
  }

  char *draw_exploration_str = getenv("DRAW_EXPLORATION");
  bool draw_exploration = false;
  if (draw_exploration_str != NULL && strlen(draw_exploration_str) > 0) {
    draw_exploration = true;
  }

  program_t program = program_from_text_filepath("puzzle.1202");
  process_t *process = instantiate_process(program);

  dynamic_grid_t *grid = create_dynamic_grid();

  coord_t origin = {0, 0};
  coord_t target;
  int64_t dist = visit_node(origin, 0, grid, process, NO_DIRECTION, &target, draw_exploration);
  printf("Distance: %d\n", dist);
  printf("Target location: (%jd, %jd)\n", target.x, target.y);

  printf("Time to fill: %d\n", fill_with_oxygen(grid, target));

  destroy_dynamic_grid(grid);
  destroy_process(process);
  destroy_program(program);
  return 0;
}
