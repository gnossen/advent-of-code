#include "stack.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

stack_t *create_stack() {
  stack_t *stack = malloc(sizeof(stack_t));
  stack->capacity = INITIAL_STACK_SIZE;
  stack->data = malloc(sizeof(coord_t) * stack->capacity);
  stack->length = 0;
  return stack;
}

void destroy_stack(stack_t *stack) {
  free(stack->data);
  free(stack);
}

static void maybe_grow_stack(stack_t *stack) {
  if (stack->length == stack->capacity) {
    stack->capacity *= 2;
    coord_t *new_data = malloc(sizeof(coord_t) * stack->capacity);
    memcpy(new_data, stack->data, sizeof(coord_t) * stack->length);
    coord_t *old_data = stack->data;
    stack->data = new_data;
    free(old_data);
  }
}

void stack_push(stack_t *stack, coord_t coord) {
  maybe_grow_stack(stack);
  stack->data[stack->length++] = coord;
}

coord_t stack_pop(stack_t *stack) {
  if (stack->length == 0) {
    fprintf(stderr, "Attempt to pop from empty queue\n");
    exit(1);
  }
  return stack->data[--stack->length];
}

coord_t stack_peek(stack_t *stack) {
  if (stack->length == 0) {
    fprintf(stderr, "Attempt to peek on empty queue\n");
    exit(1);
  }
  return stack->data[stack->length - 1];
}


bool stack_empty(stack_t *stack) {
  return stack->length == 0;
}
