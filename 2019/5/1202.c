#include "1202.h"
#include <stdlib.h>
#include <string.h>

static const int64_t k_add_op = 1;
static const int64_t k_mult_op = 2;
static const int64_t k_input_op = 3;
static const int64_t k_output_op = 4;
static const int64_t k_jump_if_true_op = 5;
static const int64_t k_jump_if_false_op = 6;
static const int64_t k_less_than_op = 7;
static const int64_t k_equals_op = 8;
static const int64_t k_halt_op = 99;

static const size_t k_max_program_size = 1 << 14;
static const size_t k_io_size = 1 << 10;

// Can be used on both programs *and* processes.
static void advance(int64_t *data, size_t len, int64_t **ip, size_t steps) {
  if ((*ip - data) + steps > len + 1) {
    fprintf(stderr,
            "Malformed program. Attempt to advance to position %d when buffer "
            "is only of size %d.\n",
            (*ip - data) + steps, len);
    exit(1);
  }
  (*ip) += steps;
}

static int is_numerical(char c) { return c >= '0' && c <= '9'; }

int64_t *program_end(program_t program) { return program.data + program.len; }

void pretty_print_program(FILE *f, program_t program, int print_offsets) {
  int64_t *ip = program.data;
  do {
    if (print_offsets) {
      fprintf(f, "%d: ", ip - program.data);
    }
    int64_t this_opcode = opcode(binary_to_bcd(*ip));
    if (this_opcode == k_halt_op) {
      advance(program.data, program.len, &ip, 1);
      fprintf(f, "%d\n", *(ip - 1));
    } else if (this_opcode == k_input_op || this_opcode == k_output_op) {
      advance(program.data, program.len, &ip, 2);
      fprintf(f, "%d, %d\n", *(ip - 2), *(ip - 1));
    } else if (this_opcode == k_add_op || this_opcode == k_mult_op || this_opcode == k_less_than_op) {
      advance(program.data, program.len, &ip, 4);
      fprintf(f, "%d, %d, %d, %d\n", *(ip - 4), *(ip - 3),
              *(ip - 2), *(ip - 1));
    } else if (this_opcode == k_jump_if_true_op || this_opcode == k_jump_if_false_op) {
      advance(program.data, program.len, &ip, 3);
      fprintf(f, "%d, %d, %d\n", *(ip - 3), *(ip - 2), *(ip - 1));
    } else {
      size_t upper = MIN(4, program.len - (ip - program.data));
      for (size_t i = 0; i < upper; ++i) {
        advance(program.data, program.len, &ip, 1);
        fprintf(f, "%d", *(ip - 1));
        if (ip != program_end(program) && i != (upper - 1)) {
          fprintf(f, ", ");
        }
      }
      fprintf(f, "\n");
    }
  } while (ip < program_end(program));
}

void debug_print_process(const char* path, process_t* process) {
  FILE *f = fopen(path, "w");
  program_t faux_program = {
    .data = process->data,
    .len = process->len,
    .buffer_len = process->buffer_len,
  };
  fprintf(f, "IP: %d\n", process->ip - process->data);
  pretty_print_program(f, faux_program, 1);
  fclose(f);
}

program_t program_from_text_file(FILE *f) {
  int64_t *data = malloc(sizeof(int64_t) * k_max_program_size);
  int64_t *rp = data;
  char buffer[k_buffer_size];
  char *bp = buffer;
  int cur = '0';
  size_t len = 0;
  while (cur != EOF) {
    // Read number.
    while (1) {
      cur = fgetc(f);
      if (is_numerical(cur) || (bp == buffer && cur == '-')) {
        *(bp++) = (char)cur;
      } else {
        // Okay to drop this character.
        *bp = '\0';
        *(rp++) = atoi(buffer);
        bp = buffer;
        break;
      }
    }

    if (cur == EOF)
      break;

    // Skip over non-meaningful characters
    while (1) {
      cur = fgetc(f);
      if (cur == EOF) {
        break;
      } else if (is_numerical(cur) || (bp == buffer && cur == '-')) {
        *(bp++) = (char)cur;
        break;
      }
    }
  }
  // NOTE: Shrink down to the smallest possible size for maximum performance.
  size_t program_len = rp - data;
  data = realloc(data, sizeof(int64_t) * program_len);
  program_t program = {
      .data = data, .len = program_len, .buffer_len = program_len};
  return program;
}

program_t program_from_text_filepath(const char *path) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    fprintf(stderr, "Unable to open %s.\n", path);
    exit(1);
  }
  program_t program = program_from_text_file(f);
  fclose(f);
  return program;
}

static int64_t add_op(process_t *process, int64_t a, int64_t b) {
  return a + b;
}

static int64_t mult_op(process_t *process, int64_t a, int64_t b) {
  return a * b;
}

static int64_t less_than_op(process_t *process, int64_t a, int64_t b) {
  return (a < b) ? 1 : 0;
}

typedef int64_t (*binary_op)(process_t *process, int64_t a, int64_t b);

static int64_t get_arg_value(process_t *process, int64_t mode, uint64_t arg) {
  int64_t value;
  if (mode == POSITION_MODE) {
    return *(process->data + arg);
  } else {
    // IMMEDIATE_MODE
    return arg;
  }
}

static void perform_binary_op(process_t *process, int64_t instruction,
                              binary_op op) {
  advance(process->data, process->len, &(process->ip), 4);
  if (*(process->ip - 1) >= process->len) {
    fprintf(stderr,
            "Attempt to modify location %d from instruction at location %d.",
            *(process->ip - 1), process->ip - 4);
    exit(1);
  }
  int64_t arg1 =
      get_arg_value(process, argument_mode(instruction, 0), *(process->ip - 3));
  int64_t arg2 =
      get_arg_value(process, argument_mode(instruction, 1), *(process->ip - 2));
  if (argument_mode(instruction, 2) != 0) {
    fprintf(stderr, "Invalid instruction. Encountered output offset not in position mode.\n");
    exit(1);
  }
  *(process->data + *(process->ip - 1)) = op(process, arg1, arg2);
}

process_status execute(process_t *process) {
  while (process->ip < process->data + process->len) {
    #ifdef DEBUG
    const size_t debug_path_len = 256;
    char debug_path[debug_path_len];
    snprintf(debug_path, debug_path_len, "debug/%d.out", process->step); 
    debug_print_process(debug_path, process); 
    #endif

    int64_t bytecode = binary_to_bcd(*(process->ip));
    if (opcode(bytecode) == k_add_op) {
      perform_binary_op(process, bytecode, add_op);
    } else if (opcode(bytecode) == k_mult_op) {
      perform_binary_op(process, bytecode, mult_op);
    } else if (opcode(bytecode) == k_input_op) {
      if (buffer_empty(process->input)) {
        return AWAITING_READ;
      }
      // TODO: Implement argument modes for this instruction.
      advance(process->data, process->len, &(process->ip), 2);
      int64_t read_value = buffer_read(process->input);
      int64_t offset = *(process->ip - 1);
      fprintf(stderr, "Inputting read value %jd at location %jd.\n", read_value, offset);
      *(process->data + offset) = read_value;
    } else if (opcode(bytecode) == k_output_op) {
      if (buffer_full(process->output)) {
        return AWAITING_WRITE;
      }
      advance(process->data, process->len, &(process->ip), 2);
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0),
                                  *(process->ip - 1));
      buffer_write(process->output, arg);
    } else if (opcode(bytecode) == k_jump_if_true_op) {
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0), *(process->ip + 1));
      int64_t jmp_pos = get_arg_value(process, argument_mode(bytecode, 1), *(process->ip + 2));
      
      if (arg != 0) {
        process->ip = process->data + jmp_pos;
      } else {
        advance(process->data, process->len, &(process->ip), 3);
      }
    } else if (opcode(bytecode) == k_jump_if_false_op) {
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0), *(process->ip + 1));
      int64_t jmp_pos = get_arg_value(process, argument_mode(bytecode, 1), *(process->ip + 2));
      
      if (arg == 0) {
        process->ip = process->data + jmp_pos;
      } else {
        advance(process->data, process->len, &(process->ip), 3);
      }
    } else if (opcode(bytecode) == k_less_than_op) {
      perform_binary_op(process, bytecode, less_than_op);
    } else if (opcode(bytecode) == k_halt_op) {
      return HALTED;
    } else {
      fprintf(stderr, "Unrecognized opcode %jd in instruction %jd at location %d.\n",
              opcode, *(process->ip), process->ip - process->data);
      exit(1);
    }
    process->step += 1;
  }
  return HALTED;
}

process_t *instantiate_process_from_buffer(program_t program, int64_t *buffer,
                                           size_t buffer_len) {
  if (buffer_len < program.len) {
    fprintf(stderr,
            "Attempt to execute program of length %d in buffer of length %d.\n",
            program.len, buffer_len);
    exit(1);
  }
  memcpy(buffer, program.data, sizeof(int64_t) * program.len);
  process_t *process = malloc(sizeof(process_t));
  process->data = buffer;
  process->len = program.len;
  process->buffer_len = buffer_len;
  process->ip = buffer;
  process->input = make_buffer(k_io_size);
  process->output = make_buffer(k_io_size);
  process->step = 0;
  return process;
}

void reset_process(const program_t program, process_t *process) {
  memcpy(process->data, program.data, sizeof(int64_t) * program.len);
  process->ip = process->data;
  buffer_clear(process->input);
  buffer_clear(process->output);
}

// NOTE: It is the caller's responsibility to free the memory in this process.
process_t *instantiate_process(program_t program) {
  int64_t *buffer = malloc(sizeof(int64_t) * program.len);
  return instantiate_process_from_buffer(program, buffer, program.len);
}

void destroy_process(process_t *process) {
  free(process->data);
  destroy_buffer(process->input);
  destroy_buffer(process->output);
  free(process);
}

void destroy_program(program_t program) { free(program.data); }

buffer_t *make_buffer(size_t size) {
  int64_t *data = malloc(sizeof(int64_t) * size);
  buffer_t *buffer = malloc(sizeof(buffer_t));
  buffer->data = data;
  buffer->read_index = 0;
  buffer->write_index = 0;
  buffer->len = size;
  return buffer;
}

void destroy_buffer(buffer_t *buffer) {
  free(buffer->data);
  free(buffer);
}

bool buffer_empty(const buffer_t *buffer) {
  return buffer->read_index == buffer->write_index;
}

bool buffer_full(const buffer_t *buffer) {
  return buffer->write_index - buffer->read_index == buffer->len;
}

void buffer_clear(buffer_t *buffer) {
  buffer->read_index = 0;
  buffer->write_index = 0;
}

int64_t buffer_read(buffer_t *buffer) {
  return buffer->data[buffer->read_index++ % buffer->len];
}

void buffer_write(buffer_t *buffer, int64_t val) {
  buffer->data[buffer->write_index++ % buffer->len] = val;
}

int64_t argument_mode(uint64_t instruction, size_t argument) {
  /* The trick to isolate the four bits is to shift the left end of the content
   * to the left edge of the quadword, then to shift the right all the way to
   * the right end of the quadword, erasing everything else.
   */
  return (instruction << (64 -
                          (ARG_MODE_BITS * (argument + 1) + OPCODE_BITS))) >>
         (64 - ARG_MODE_BITS);
}

int64_t opcode(uint64_t instruction) { return instruction & OPCODE_MASK; }

int64_t decimal_to_bytecode(const char *decimal) {
  int64_t instruction;
  size_t p = strlen(decimal) - 1;
  instruction = decimal[p] - '0';
  if (--p == -1)
    return instruction;
  instruction += (decimal[p] - '0') * 10;
  if (--p == -1)
    return instruction;
  instruction |= ((decimal[p] - '0') << OPCODE_BITS);
  if (--p == -1)
    return instruction;
  instruction |= ((decimal[p] - '0') << (OPCODE_BITS + ARG_MODE_BITS));
  if (--p == -1)
    return instruction;
  instruction |= ((decimal[p] - '0') << (OPCODE_BITS + 2 * ARG_MODE_BITS));
  return instruction;
}

void bytecode_to_decimal(int64_t instruction, char *decimal,
                         size_t buffer_len) {
  decimal[0] = '\0';
  size_t digits = 0;
  int64_t mode;
  size_t start = OPCODE_BITS + 2 * ARG_MODE_BITS;
  for (int i = 2; i >= 0; i--) {
    mode = argument_mode(instruction, i);
    if (digits != 0 || mode != 0) {
      decimal[digits++] = '0' + mode;
    }
    start -= ARG_MODE_BITS;
  }
  int64_t opc = opcode(instruction);
  int64_t ones = opc % 10;
  int64_t tens = (opc - ones) / 10;
  if (digits != 0 || tens != 0) {
    decimal[digits++] = '0' + tens;
  }
  decimal[digits++] = '0' + ones;
  decimal[digits++] = '\0';
}

int64_t binary_to_bcd(int64_t instruction) {
  int64_t bytecode;
  int64_t temp;
  bytecode = instruction % 100;
  instruction -= bytecode;
  int64_t radix = 100;
  for (size_t i = 0; i < 3; ++i) {
    temp = instruction % (radix * 10);
    instruction -= temp;
    bytecode |= (temp / radix) << (OPCODE_BITS + i * ARG_MODE_BITS);
    radix *= 10;
  }
  return bytecode;
}
