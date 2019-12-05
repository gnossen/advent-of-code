#include "1202.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void drain_output(buffer_t *output) {
  while (!buffer_empty(output)) {
    printf("%d\n", buffer_read(output));
  }
}

void run_program_loop(process_t *process) {
  process_status status;
  while ((status = execute(process)) != HALTED) {
    if (status == AWAITING_READ) {
      uint64_t input_val;
      scanf("%d\n", &input_val);
      buffer_write(process->input, input_val);
    } else if (status == AWAITING_WRITE) {
      drain_output(process->output);
    }
  }
  drain_output(process->output);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(1);
  }
  program_t program = program_from_text_filepath(argv[1]);
  process_t *process = instantiate_process(program);
  run_program_loop(process);
  destroy_program(program);
  destroy_process(process);
  return 0;
}
