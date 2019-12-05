#ifndef __1202__
#define __1202__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static const size_t k_buffer_size = 1024;

#define MIN(a, b) ((a > b) ? b : a)

typedef struct program_t {
  uint64_t *data;
  size_t len;
  size_t buffer_len;
} program_t;

typedef program_t process_t;

uint64_t *program_end(program_t program);

process_t instantiate_process_from_buffer(program_t program, uint64_t *buffer,
                                          size_t buffer_len);

// NOTE: It is the caller's responsibility to free the memory in this process.
process_t instantiate_process(program_t program);

void reset_process(const program_t program, process_t process);

void destroy_process(process_t process);
void destroy_program(program_t program);

void pretty_print_program(FILE *f, program_t program);

// NOTE: It is the caller's responsibility to free the program's memory.
program_t program_from_text_file(FILE *f);

// NOTE: It is the caller's responsibility to free the program's memory.
program_t program_from_text_filepath(const char *path);

void execute_program(process_t process);

#endif
