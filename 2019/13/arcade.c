#include "1202.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RETURN_IF_FALSE(expr) if (!(expr)) {return false;}

// TODO: Consider making variable sizee.
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 64

// Default tile is empty.
#define SCREEN_DEFAULT_TYPE 0

#define PADDLE_TYPE 3
#define BALL_TYPE 4

typedef struct object_t {
  int64_t x;
  int64_t y;
  int64_t type;
} object_t;

typedef struct arcade_iterator_t {
  process_t * process;
  process_status status;
  bool started;
} arcade_iterator_t;

arcade_iterator_t *create_arcade_iterator(process_t *process) {
  arcade_iterator_t *it = malloc(sizeof(arcade_iterator_t));
  it->process = process;
  it->status = HALTED;
  it->started = false;
  return it;
}

void destroy_arcade_iterator(arcade_iterator_t *it) {
  free(it);
}

bool internal_next_int(arcade_iterator_t *it, int64_t * next) {
  if (buffer_empty(it->process->output)) {
    if (it->status == AWAITING_READ) {
      return false;
    }

    if (!it->started || it->status == AWAITING_WRITE) {
      it->status = execute(it->process);
      if (it->status == HALTED) {
        return false;
      }
      if (it->status == AWAITING_WRITE && buffer_empty(it->process->output)) {
        fprintf(stderr, "Process returned AWAITING_READ without returning any output.\n");
        exit(1);
      }
      it->started = true;
    }
  }

  *next = buffer_read(it->process->output);
  return true;
}

/* Takes one object from the buffer, setting object to the value, potentially
 * running the process to do so. Returns true if an object has been returned.
 * False if the process is awaiting input.
 */
bool next_object(arcade_iterator_t *it, object_t *object) {
  RETURN_IF_FALSE(internal_next_int(it, &(object->x)));
  RETURN_IF_FALSE(internal_next_int(it, &(object->y)));
  RETURN_IF_FALSE(internal_next_int(it, &(object->type)));
  return true;
}

typedef struct screen_t {
  // Arranged by rows.
  int64_t objects[SCREEN_WIDTH * SCREEN_HEIGHT];
  int64_t score;
  object_t paddle;
  object_t ball;
} screen_t;

void clear_screen(screen_t *screen) {
  memset(&(screen->objects), SCREEN_DEFAULT_TYPE, sizeof(int64_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
}

screen_t* create_screen() {
  screen_t *screen = malloc(sizeof(screen_t));
  clear_screen(screen);
  screen->score = 0;
  screen->paddle.x = 0;
  screen->paddle.y = 0;
  screen->ball.x = 0;
  screen->ball.y = 0;
  return screen;
}

void print_screen(screen_t *screen) {
  printf("score: %jd\n", screen->score);
  for (size_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    if ((i != 0) && (i %  SCREEN_WIDTH) == 0) {
      printf("\n");
    }
    int64_t type = screen->objects[i];
    char *tile;
    switch (type) {
      case 0:
        tile = " "; // Empty
        break;
      case 1:
        tile = "#"; // Wall
        break;
      case 2:
        tile = "█"; // Block
        break;
      case 3:
        tile = "🮄"; // Paddle
        break;
      case 4:
        tile = "◌"; // Ball
        break;
      default:
        tile = "?"; // Unknown
    }
    printf("%s", tile);
  }

  printf("\n");
}

void destroy_screen(screen_t *screen) {
  free(screen);
}

void place_object(screen_t *screen, object_t object) {
  if (object.x < 0 || object.y < 0 || object.x >= SCREEN_WIDTH || object.y >= SCREEN_HEIGHT) {
    fprintf(stderr, "Attempt to place out-of-bounds object of type %jd at location (%jd, %jd).\n", object.type, object.x, object.y);
    exit(1);
  }
  screen->objects[object.y * SCREEN_WIDTH + object.x] = object.type;
}

void update_screen(process_t *process, screen_t *screen) {
  object_t object;

  // TODO: Figure out a way not to do any allocations in the main loop.
  arcade_iterator_t *it = create_arcade_iterator(process);

  while (next_object(it, &object)) {
    // printf("Object\n  x: %jd\n  y: %jd\n  type: %jd\n", object.x, object.y, object.type);
    if (object.type == PADDLE_TYPE) {
      screen->paddle = object;
    } else if (object.type == BALL_TYPE) {
      screen->ball = object;
    }

    if (object.x == -1 && object.y == 0) {
      screen->score = object.type;
    } else if (object.x >= 0 && object.y >= 0) {
      // For the moment, just don't print anything negative.
      place_object(screen, object);
    }
  }

  destroy_arcade_iterator(it);
}

int64_t arcade_get_input() {
  char c = '0';

  while (true) {
    int i = getchar();
    c = (char)i;
    if (c == '\n') { continue; }
    if (c == 'a') { return -1; } // left
    if (c == 'd') { return 1; } // right
    return 0; // neutral
  }
}

uint64_t screen_get_movement(screen_t *screen) {
  if (screen->ball.x == screen->paddle.x) {
    return 0;
  } else if (screen->ball.x > screen->paddle.x) {
    return 1;
  } else {
    return -1;
  }
}

int main(int argc, char **argv) {
    bool manual = false;
    char *arcade_manual_str = getenv("ARCADE_MANUAL");
    if (arcade_manual_str != NULL && strlen(arcade_manual_str) > 0) {
      manual = true;
    }

    program_t program = program_from_text_filepath("puzzle.1202");

    // Set quarters to 2.
    program.data[0] = 2;

    process_t *process = instantiate_process(program);

    screen_t *screen = create_screen();


    while (process->status != HALTED) {
      update_screen(process, screen);
      print_screen(screen);

      int64_t input;
      if (manual) {
        input = arcade_get_input();
      } else {
        input = screen_get_movement(screen);
        usleep(62500);
      }
      buffer_write(process->input, input);
    }

    printf("Final score: %jd\n", screen->score);

    destroy_screen(screen);

    destroy_process(process);
    destroy_program(program);
    return 0;
}
