#include "1202.h"
#include <stdlib.h>

static const int64_t k_max_input = 100;

int64_t calculate(int64_t a, int64_t b, uint64_t *program, size_t program_len,
                  uint64_t *buffer, size_t buffer_len) {
  program[1] = a;
  program[2] = b;
  execute_in_buffer(program, program_len, buffer, buffer_len);
  return buffer[0];
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s program desired_output\n", argv[0]);
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
  int64_t desired = atoi(argv[2]);
  uint64_t buffer[k_buffer_size];
  for (size_t i = 0; i < k_max_input; ++i) {
    for (size_t j = 0; j < k_max_input; ++j) {
      int64_t output =
          calculate(i, j, program, program_len, buffer, k_buffer_size);
      if (output == desired) {
        printf("%d\n", 100 * i + j);
        return 0;
      }
    }
  }
  fprintf(stderr, "Failed to find input resulting in %d.\n", desired);
  return 1;
}
