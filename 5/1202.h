#ifndef __1202__
#define __1202__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static const size_t k_buffer_size = 1024;

#define MIN(a, b) ((a > b) ? b : a)

// A basic circular array.
typedef struct buffer_t {
  int64_t *data;

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
void buffer_clear(buffer_t *buffer);

// NOTE: UB results when buffer_read is called on an empty buffer.
int64_t buffer_read(buffer_t *buffer);

// NOTE: UB results when buffer_write is called on a full buffer.
void buffer_write(buffer_t *buffer, int64_t val);

typedef struct program_t {
  int64_t *data;
  size_t len;
  size_t buffer_len;
} program_t;

// typedef program_t process_t;

typedef struct process_t {
  int64_t *data;
  size_t len;
  size_t buffer_len;
  int64_t *ip;
  buffer_t *input;
  buffer_t *output;
} process_t;

int64_t *program_end(program_t program);

process_t *instantiate_process_from_buffer(program_t program, int64_t *buffer,
                                           size_t buffer_len);

// NOTE: It is the caller's responsibility to free the memory in this process.
process_t *instantiate_process(program_t program);

void reset_process(const program_t program, process_t *process);

void destroy_process(process_t *process);

void destroy_program(program_t program);

void pretty_print_program(FILE *f, program_t program);

// NOTE: It is the caller's responsibility to free the program's memory.
program_t program_from_text_file(FILE *f);

// NOTE: It is the caller's responsibility to free the program's memory.
program_t program_from_text_filepath(const char *path);

typedef enum process_status {
  HALTED,
  AWAITING_WRITE,
  AWAITING_READ
} process_status;

process_status execute(process_t *process);

static const size_t OPCODE_BITS = 7;
static const size_t ARG_MODE_BITS = 4;
static const int64_t OPCODE_MASK = (1 << (OPCODE_BITS + 1)) - 1;
static const int64_t POSITION_MODE = 0;
static const int64_t IMMEDIATE_MODE = 1;

/* Instructions are translated from a textual decimal representation to a
 * binary representation.
 *
 * The least significant 7 bits are reserved for the opcode. This makes 128
 * possibilities, which accommodates the highest possible decimal value of 99.
 * 
 * After that, each group of four bits represents the mode of each argument.
 * For example, 1002 would be converted as follows:
 *
 *   000000000000000000000000000000000000000000000 | 0000 | 0001 | 0000 | 0000010
 *   UNUSED                                        | ARG3 | ARG2 | ARG1 | OPCODE
 */

// NOTE: Argument index is zero based.
int64_t argument_mode(uint64_t instruction, size_t argument);

int64_t opcode(uint64_t instruction);

int64_t decimal_to_bytecode(const char* decimal);

#endif
