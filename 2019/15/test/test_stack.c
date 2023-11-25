#include "../stack.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

void test_basic() {
  stack_t *stack = create_stack();
  coord_t to_add[] = {
    {20, 20},
    {-20, 20},
    {1000, 0},
    {0, 0},
    {-19, 19},
    {0, 1000},
  };
  size_t to_add_len = sizeof(to_add) / sizeof(coord_t);
  for (size_t i = 0; i < to_add_len; ++i) {
    stack_push(stack, to_add[i]);
    assert(stack->length == i + 1);
  }
  for (size_t i = 0; i < to_add_len; ++i) {
    coord_t popped = stack_pop(stack);
    assert(popped.x == to_add[to_add_len - i - 1].x);
    assert(popped.y == to_add[to_add_len - i - 1].y);
    assert(stack->length == to_add_len - i - 1);
  }

  destroy_stack(stack);
}

int main(int argc, char **argv) {
  test_basic();
  printf("SUCCESS\n");
  return 0;
}
