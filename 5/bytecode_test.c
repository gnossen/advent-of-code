#include "1202.h"
#include <stdlib.h>
#include <assert.h>

void test_decimal_to_bytecode() {
  uint64_t instruction = decimal_to_bytecode("01002");
  assert(instruction == 0x802);
  assert(opcode(instruction) == 2);
  assert(argument_mode(instruction, 0) == 0);
  assert(argument_mode(instruction, 1) == 1);
  assert(argument_mode(instruction, 2) == 0);
}

int main(int argc, char ** argv) {
  test_decimal_to_bytecode();
  printf("SUCCESS\n");
  return 0;
}
