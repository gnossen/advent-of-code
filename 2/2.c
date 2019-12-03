#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const uint64_t k_add_op = 1;
const uint64_t k_mult_op = 2;
const uint64_t k_halt_op = 99;

const size_t k_buffer_size = 1024;

#define MIN(a, b) ((a > b) ? b : a)

static uint64_t initial[] = {1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50};

void advance(const uint64_t *start, const uint64_t **ip, size_t steps,
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

int is_numerical(char c) { return c >= '0' && c <= '9'; }

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

// NOTE: Mutates program in-place. Copy beforehand if needed.
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

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(1);
  }
  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "Unable to open %s.\n", argv[1]);
    exit(1);
  }
  uint64_t program[k_buffer_size];
  size_t program_len;
  parse_program(f, k_buffer_size, program, &program_len);
  fclose(f);
  execute_program(program, program_len);
  pretty_print_program(stdout, program, program_len);
  return 0;
}
