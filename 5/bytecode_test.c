#include "1202.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 10

void test_decimal_to_bytecode() {
  uint64_t instruction = decimal_to_bytecode("01002");
  assert(instruction == 0x802);
  assert(opcode(instruction) == 2);
  assert(argument_mode(instruction, 0) == 0);
  assert(argument_mode(instruction, 1) == 1);
  assert(argument_mode(instruction, 2) == 0);
}

void test_bytecode_to_decimal() {
  char buffer[BUF_LEN];
  bytecode_to_decimal(0x802, buffer, BUF_LEN);
  assert(!strcmp("1002", buffer));
}

int main(int argc, char **argv) {
  test_decimal_to_bytecode();
  test_bytecode_to_decimal();
  printf("SUCCESS\n");
  return 0;
}
