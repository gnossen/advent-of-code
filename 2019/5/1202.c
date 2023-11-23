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
static const int64_t k_adjust_relative_base_op = 9;
static const int64_t k_halt_op = 99;

static const size_t k_max_program_size = 1 << 14;
static const size_t k_io_size = 1 << 10;

// Can be used on both programs *and* processes.
static void advance(size_t len, int64_t *ip_offset, size_t steps) {
  (*ip_offset) += steps;
}

static int is_numerical(char c) { return c >= '0' && c <= '9'; }

void pretty_print_program(FILE *f, program_t program, int print_offsets) {
  int64_t ip_offset = 0;
  do {
    if (print_offsets) {
      fprintf(f, "%d: ", ip_offset);
    }
    int64_t this_opcode = opcode(binary_to_bcd(*(program.data + ip_offset)));
    if (this_opcode == k_halt_op) {
      advance(program.len, &ip_offset, 1);
      fprintf(f, "%jd\n", *(program.data + ip_offset - 1));
    } else if (this_opcode == k_input_op || this_opcode == k_output_op || this_opcode == k_adjust_relative_base_op) {
      advance(program.len, &ip_offset, 2);
      fprintf(f, "%jd, %jd\n", *(program.data + ip_offset - 2), *(program.data + ip_offset - 1));
    } else if (this_opcode == k_add_op || this_opcode == k_mult_op || this_opcode == k_less_than_op || this_opcode == k_equals_op) {
      advance(program.len, &ip_offset, 4);
      fprintf(f, "%jd, %jd, %jd, %jd\n", *(program.data + ip_offset - 4), *(program.data + ip_offset - 3),
              *(program.data + ip_offset - 2), *(program.data + ip_offset - 1));
    } else if (this_opcode == k_jump_if_true_op || this_opcode == k_jump_if_false_op) {
      advance(program.len, &ip_offset, 3);
      fprintf(f, "%jd, %jd, %jd\n", *(program.data + ip_offset - 3), *(program.data + ip_offset - 2), *(program.data + ip_offset - 1));
    } else {
      size_t upper = MIN(4, program.len - (ip_offset));
      for (size_t i = 0; i < upper; ++i) {
        advance(program.len, &ip_offset, 1);
        fprintf(f, "%jd", *(program.data + ip_offset - 1));
        if (ip_offset != program.len && i != (upper - 1)) {
          fprintf(f, ", ");
        }
      }
      fprintf(f, "\n");
    }
  } while (ip_offset < program.len);
}

void debug_print_process(const char* path, process_t* process) {
  FILE *f = fopen(path, "w");
  program_t faux_program = {
    .data = process->data,
    .len = process->len,
    .buffer_len = process->buffer_len,
  };
  fprintf(f, "IP: %d\n", process->ip_offset);
  fprintf(f, "Relative base: %d\n", process->relative_base);
  pretty_print_program(f, faux_program, 1);
  fclose(f);
}

static size_t smallest_power_of_2_greater_than(size_t n) {
    size_t i = 1;
    while (i <= n) { i <<= 1; }
    return i;
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
        *(rp++) = strtoll(buffer, NULL, 10);
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
  // Shrink down to the smallest power of 2 big enough to contain the program.
  size_t program_len = rp - data;
  size_t buffer_size = smallest_power_of_2_greater_than(program_len);
  data = realloc(data, sizeof(int64_t) * buffer_size);

  // Set everything beyond the program to 0.
  memset(data + program_len, 0, sizeof(int64_t) * (buffer_size - program_len));
  program_t program = {
      .data = data, .len = program_len, .buffer_len = buffer_size};
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

static int64_t equals_op(process_t *process, int64_t a, int64_t b) {
  return (a == b) ? 1 : 0;
}

typedef int64_t (*binary_op)(process_t *process, int64_t a, int64_t b);

void process_buffer_grow(process_t *process, size_t new_size) {
    size_t old_size = process->buffer_len;
    process->data = realloc(process->data, sizeof(int64_t) * new_size);
    memset(process->data + old_size, 0, sizeof(int64_t) * (new_size - old_size));
    process->buffer_len = new_size;
}

void process_grow(process_t *process, int64_t offset) {
    if (offset > process->buffer_len) {
        process_buffer_grow(process, smallest_power_of_2_greater_than(offset));
    }
    process->len = offset + 1;
}

void process_write(process_t *process, int64_t offset, int64_t value) {
  if (offset < 0) {
    fprintf(stderr, "Attempt to write value %ld at negative offset %ld.\n", value, offset);
    exit(1);
  }
  if (offset >= process->len) {
      process_grow(process, offset);
  }

  *(process->data + offset) = value;
}

int64_t process_read(process_t *process, int64_t offset) {
  if (offset < 0) {
    fprintf(stderr, "Attempt to read from negative offset %ld.\n", offset);
    exit(1);
  }
  if (offset >= process->len) {
    process_grow(process, offset);
  }

  return *(process->data + offset);
}

static int64_t get_arg_value(process_t *process, int64_t mode, uint64_t arg) {
  int64_t value;
  if (mode == POSITION_MODE) {
    return process_read(process, arg);
  } else if (mode == IMMEDIATE_MODE) {
    return arg;
  } else if (mode == RELATIVE_MODE) {
    return process_read(process, arg + process->relative_base);
  } else {
    fprintf(stderr, "Encountered unknown argument mode %jd\n", mode);
    exit(1);
  }
}

static void perform_binary_op(process_t *process, int64_t instruction,
                              binary_op op) {
  advance(process->len, &(process->ip_offset), 4);
  int64_t arg1 =
      get_arg_value(process, argument_mode(instruction, 0), process_read(process, process->ip_offset - 3));
  int64_t arg2 =
      get_arg_value(process, argument_mode(instruction, 1), process_read(process, process->ip_offset - 2));

  // TODO: Dedupe with input instruction.
  int64_t offset = process_read(process, process->ip_offset - 1);
  int64_t offset_mode = argument_mode(instruction, 2);
  if (offset_mode == POSITION_MODE) {
    process_write(process, offset, op(process, arg1, arg2));
  } else if (offset_mode == RELATIVE_MODE) {
    process_write(process, offset + process->relative_base, op(process, arg1, arg2));
  } else {
    fprintf(stderr, "Invalid instruction. Encountered output offset with unknown mode %jd.\n", offset_mode);
    exit(1);
  }
}


process_status execute(process_t *process) {
  while (process->ip_offset < process->len) {
    char *debug1202 = getenv("DEBUG1202");
    if (debug1202 != NULL && strlen(debug1202) > 0) {
      const size_t debug_path_len = 256;
      char debug_path[debug_path_len];
      snprintf(debug_path, debug_path_len, "debug/%d.out", process->step);
      debug_print_process(debug_path, process);
    }

    int64_t instruction = process_read(process, process->ip_offset);
    int64_t bytecode = binary_to_bcd(instruction);
    int64_t this_opcode = opcode(bytecode);
    if (this_opcode == k_add_op) {
      perform_binary_op(process, bytecode, add_op);
    } else if (this_opcode == k_mult_op) {
      perform_binary_op(process, bytecode, mult_op);
    } else if (this_opcode == k_input_op) {
      if (buffer_empty(process->input)) {
        return AWAITING_READ;
      }
      advance(process->len, &(process->ip_offset), 2);
      int64_t read_value = buffer_read(process->input);
      int64_t offset = process_read(process, process->ip_offset - 1);
      int64_t mode = argument_mode(bytecode, 0);
      if (mode == POSITION_MODE) {
        process_write(process, offset, read_value);
      } else if (mode == RELATIVE_MODE) {
        process_write(process, offset + process->relative_base, read_value);
      } else {
        fprintf(stderr, "Encountered unsupported argument mode %jd\n", mode);
        exit(1);
      }
    } else if (this_opcode == k_output_op) {
      if (buffer_full(process->output)) {
        return AWAITING_WRITE;
      }
      advance(process->len, &(process->ip_offset), 2);
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0),
                                  process_read(process, process->ip_offset - 1));
      buffer_write(process->output, arg);
    } else if (this_opcode == k_adjust_relative_base_op) {
      advance(process->len, &(process->ip_offset), 2);
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0),
                                  process_read(process, process->ip_offset - 1));
      process->relative_base += arg;
    } else if (this_opcode == k_jump_if_true_op) {
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0), process_read(process, process->ip_offset + 1));
      int64_t jmp_pos = get_arg_value(process, argument_mode(bytecode, 1), process_read(process, process->ip_offset + 2));

      if (arg != 0) {
        process->ip_offset = jmp_pos;
      } else {
        advance(process->len, &(process->ip_offset), 3);
      }
    } else if (this_opcode == k_jump_if_false_op) {
      int64_t arg = get_arg_value(process, argument_mode(bytecode, 0), process_read(process, process->ip_offset + 1));
      int64_t jmp_pos = get_arg_value(process, argument_mode(bytecode, 1), process_read(process, process->ip_offset + 2));

      if (arg == 0) {
        process->ip_offset = jmp_pos;
      } else {
        advance(process->len, &(process->ip_offset), 3);
      }
    } else if (this_opcode == k_less_than_op) {
      perform_binary_op(process, bytecode, less_than_op);
    } else if (this_opcode == k_equals_op) {
      perform_binary_op(process, bytecode, equals_op);
    } else if (this_opcode == k_halt_op) {
      return HALTED;
    } else {
      fprintf(stderr, "Unrecognized opcode %jd in instruction %jd at location %d.\n",
              this_opcode, instruction, process->ip_offset);
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
  process->ip_offset = 0;
  process->input = make_buffer(k_io_size);
  process->output = make_buffer(k_io_size);
  process->step = 0;
  process->relative_base = 0;
  return process;
}

void reset_process(const program_t program, process_t *process) {
  memcpy(process->data, program.data, sizeof(int64_t) * program.len);
  process->ip_offset = 0;
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
