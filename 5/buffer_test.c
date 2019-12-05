#include "1202.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_write() {
  buffer_t *buffer = make_buffer(4);
  assert(buffer_empty(buffer));
  buffer_write(buffer, 23);
  assert(buffer->data[0] == 23);
  buffer_write(buffer, 24);
  assert(buffer->data[1] == 24);
  destroy_buffer(buffer);
}

void test_read() {
  const size_t buffer_size = 4;
  buffer_t *buffer = make_buffer(buffer_size);
  buffer_write(buffer, 23);
  assert(buffer_read(buffer) == 23);
  assert(buffer_empty(buffer));
  for (size_t epochs = 0; epochs < 10; ++epochs) {
    for (size_t i = 0; i < buffer_size; ++i) {
      buffer_write(buffer, 24 + i);
    }
    assert(buffer_full(buffer));
    for (size_t i = 0; i < buffer_size; ++i) {
      assert(buffer_read(buffer) == 24 + i);
    }
    assert(buffer_empty(buffer));
  }
  destroy_buffer(buffer);
}

int main(int argc, char **argv) {
  test_write();
  test_read();
  return 0;
}
