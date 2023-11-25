#ifndef _STACK_H
#define _STACK_H

#include "dynamic_grid.h"

#include <stdbool.h>
#include <stddef.h>

// TODO: Pump up after testing.
#define INITIAL_STACK_SIZE 16

typedef struct stack_t {
  coord_t *data;
  size_t length;
  size_t capacity;
} stack_t;

stack_t *create_stack();
void destroy_stack(stack_t *stack);
void stack_push(stack_t *stack, coord_t coord);
coord_t stack_pop(stack_t *stack);
bool stack_empty(stack_t *stack);

#endif //  _STACK_H
