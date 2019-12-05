#include "1202.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t k_io_size = 1 << 10;

void run_program_loop(process_t process, buffer_t *input, buffer_t *output) {
  process_status status;
  while ((status = execute(process, input, output)) != HALTED) {
    if (status == AWAITING_READ) {
      uint64_t input_val;
      scanf(SCNu64 "\n", &input_val);
      buffer_write(input, input_val);
    } else if (status == AWAITING_WRITE) {
      while (!buffer_empty(output)) {
        printf(SCNu64 "\n", buffer_read(output));
      }
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(1);
  }
  program_t program = program_from_text_filepath(argv[1]);
  process_t process = instantiate_process(program);
  buffer_t *input = make_buffer(k_io_size);
  buffer_t *output = make_buffer(k_io_size);
  run_program_loop(process, input, output);
  destroy_buffer(input);
  destroy_buffer(output);
  destroy_program(program);
  destroy_process(process);
  return 0;
}
