#include "1202.h"
#include <stdlib.h>

static const int64_t k_max_input = 100;

int64_t calculate(int64_t a, int64_t b, process_t process) {
  process.data[1] = a;
  process.data[2] = b;
  execute_program(process);
  return process.data[0];
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s program desired_output\n", argv[0]);
    exit(1);
  }
  int64_t desired = atoi(argv[2]);
  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "Unable to open %s.\n", argv[1]);
    exit(1);
  }
  program_t program = program_from_text_file(f);
  fclose(f);
  process_t process = instantiate_process(program);
  for (size_t i = 0; i < k_max_input; ++i) {
    for (size_t j = 0; j < k_max_input; ++j) {
      int64_t output = calculate(i, j, process);
      if (output == desired) {
        printf("%d\n", 100 * i + j);
        return 0;
      }
      reset_process(program, process);
    }
  }
  destroy_process(process);
  destroy_program(program);
  fprintf(stderr, "Failed to find input resulting in %d.\n", desired);
  return 1;
}
