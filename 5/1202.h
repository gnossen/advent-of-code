#ifndef __1202__
#define __1202__

#include <stdbool.h>
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

// A basic circular array.
typedef struct buffer_t {
  uint64_t *data;

  // TODO: What about overflow?

  // Pointer to the next index to be read.
  size_t read_index;
  // Pointer to the next index to be written to.
  size_t write_index;

  size_t len;
} buffer_t;

// NOTE: The caller becomes responsible for freeing the buffer.
buffer_t *make_buffer(size_t size);
void destroy_buffer(buffer_t *buffer);

bool buffer_empty(const buffer_t *buffer);
bool buffer_full(const buffer_t *buffer);

// NOTE: UB results when buffer_read is called on an empty buffer.
uint64_t buffer_read(buffer_t *buffer);

// NOTE: UB results when buffer_write is called on a full buffer.
void buffer_write(buffer_t *buffer, uint64_t val);

// program_status_t execute(process_t process, buffer_t input, buffer_t output);

#endif
