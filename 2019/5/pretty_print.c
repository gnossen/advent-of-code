#include "1202.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(0);
  }
  // TODO: Also handle bytecode files.
  program_t program = program_from_text_filepath(argv[1]);
  pretty_print_program(stdout, program, 0);
  return 0;
}
