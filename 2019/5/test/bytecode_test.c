#include "../1202.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 10

void test_decimal_to_bytecode() {
  uint64_t instruction = binary_to_bcd(1002);
  assert(instruction == 0x802);
  assert(opcode(instruction) == 2);
  assert(argument_mode(instruction, 0) == 0);
  assert(argument_mode(instruction, 1) == 1);
  assert(argument_mode(instruction, 2) == 0);
}

void test_decimal_to_bytecode_2() {
  uint64_t instruction = binary_to_bcd(1101);
  assert(instruction == 0x881);
  assert(opcode(instruction) == 1);
  assert(argument_mode(instruction, 0) == 1);
  assert(argument_mode(instruction, 1) == 1);
  assert(argument_mode(instruction, 2) == 0);
}

int main(int argc, char **argv) {
  test_decimal_to_bytecode();
  test_decimal_to_bytecode_2();
  printf("SUCCESS\n");
  return 0;
}
