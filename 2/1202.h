#ifndef __1202__
#define __1202__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static const size_t k_buffer_size = 1024;

#define MIN(a, b) ((a > b) ? b : a)

void pretty_print_program(FILE *f, const uint64_t *program, size_t len);

void parse_program(FILE *f, size_t buffer_size, uint64_t *program, size_t *len);

// NOTE: Mutates program in-place. Copy beforehand if needed.
void execute_program(uint64_t *program, size_t len);

void execute_in_buffer(uint64_t *program, size_t len, uint64_t *buffer,
                       size_t buffer_len);

#endif
