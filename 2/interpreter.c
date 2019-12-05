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
  program_t program = program_from_text_file(f);
  fclose(f);
  process_t process = instantiate_process(program);
  execute_program(process);
  pretty_print_program(stdout, process);
  destroy_program(program);
  destroy_process(process);
  return 0;
}
