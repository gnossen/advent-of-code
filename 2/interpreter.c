#include "1202.h"
#include <stdlib.h>

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
