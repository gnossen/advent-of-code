#include "1202.h"
#include <stdlib.h>
#include <string.h>

static const uint64_t k_add_op = 1;
static const uint64_t k_mult_op = 2;
static const uint64_t k_halt_op = 99;

static void advance(const uint64_t *start, const uint64_t **ip, size_t steps,
                    size_t prog_len) {
  if ((*ip - start) + steps > prog_len + 1) {
    fprintf(stderr,
            "Malformed program. Attempt to advance to position %d when buffer "
            "is only of size %d.\n",
            (*ip - start) + steps, prog_len);
    exit(1);
  }
  (*ip) += steps;
}

static int is_numerical(char c) { return c >= '0' && c <= '9'; }

void pretty_print_program(FILE *f, const uint64_t *program, size_t len) {
  const uint64_t *ip = program;
  do {
    if (*ip == k_halt_op) {
      advance(program, &ip, 1, len);
      fprintf(f, "%d\n", *(ip - 1));
    } else if (*ip == k_add_op || *ip == k_mult_op) {
      advance(program, &ip, 4, len);
      fprintf(f, "%d, %d, %d, %d\n", *(ip - 4), *(ip - 3), *(ip - 2),
              *(ip - 1));
    } else {
      size_t upper = MIN(4, len - (ip - program));
      for (size_t i = 0; i < upper; ++i) {
        advance(program, &ip, 1, len);
        fprintf(f, "%d", *(ip - 1));
        if (ip != program + len) {
          fprintf(f, ", ");
        }
      }
      fprintf(f, "\n");
    }
  } while (ip < program + len);
}

void parse_program(FILE *f, size_t buffer_size, uint64_t *program,
                   size_t *len) {
  uint64_t *rp = program;
  char buffer[k_buffer_size];
  char *bp = buffer;
  int cur = '0';
  *len = 0;
  while (cur != EOF) {
    // Read number.
    while (1) {
      cur = fgetc(f);
      if (is_numerical(cur)) {
        *(bp++) = (char)cur;
      } else {
        // Okay to drop this character.
        *bp = '\0';
        *(rp++) = atoi(buffer);
        (*len)++;
        bp = buffer;
        break;
      }
    }

    if (cur == EOF)
      break;

    // Skip over non-meaningful characters
    while (1) {
      cur = fgetc(f);
      if (cur == EOF) {
        break;
      } else if (is_numerical(cur)) {
        *(bp++) = (char)cur;
        break;
      }
    }
  }
}

void execute_program(uint64_t *program, size_t len) {
  uint64_t *ip = program;
  while (ip < program + len) {
    if (*ip == k_add_op) {
      advance(program, (const uint64_t **)&ip, 4, len);
      if (*(ip - 1) >= len) {
        fprintf(
            stderr,
            "Attempt to modify location %d from instruction at location %d.",
            *(ip - 1), ip - 4);
        exit(1);
      }
      *(program + *(ip - 1)) = *(program + *(ip - 3)) + *(program + *(ip - 2));
    } else if (*ip == k_mult_op) {
      advance(program, (const uint64_t **)&ip, 4, len);
      if (*(ip - 1) >= len) {
        fprintf(
            stderr,
            "Attempt to modify location %d from instruction at location %d.",
            *(ip - 1), ip - 4);
        exit(1);
      }
      *(program + *(ip - 1)) = *(program + *(ip - 3)) * *(program + *(ip - 2));
    } else if (*ip == k_halt_op) {
      return;
    } else {
      fprintf(stderr, "Unrecognized opcode %d at location %d.\n", *ip,
              ip - program);
      exit(1);
    }
  }
}

void execute_in_buffer(uint64_t *program, size_t len, uint64_t *buffer,
                       size_t buffer_len) {
  if (buffer_len < len) {
    fprintf(stderr,
            "Attempt to execute program of length %d in buffer of length %d.\n",
            len, buffer_len);
    exit(1);
  }
  memcpy(buffer, program, sizeof(uint64_t) * len);
  execute_program(buffer, len);
}
