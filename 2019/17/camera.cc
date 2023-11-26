#include "../5/1202.h"
#include "../5/1202pp.h"

#include <iostream>

int main(int argc, char **argv) {
  Program program("puzzle.1202");
  Process process(program);

  for (int64_t val : process) {
    std::cout  << (char)val;
  }

  return 0;
}
