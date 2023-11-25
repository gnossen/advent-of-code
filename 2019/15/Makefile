EXECUTABLES=droid test/test_dynamic_grid test/test_stack

.PHONY: all
all: ${EXECUTABLES}

CFLAGS=-I../5

%.o: %.c
	gcc ${CFLAGS} -c $< -o $@

droid: droid.o ../5/1202.o dynamic_grid.o stack.o
	gcc ${CFLAGS} $^ -o $@

test/test_dynamic_grid: test/test_dynamic_grid.o dynamic_grid.o
	gcc ${CFLAGS} $^ -o $@

test/test_stack: test/test_stack.o stack.o
	gcc ${CFLAGS} $^ -o $@

.PHONY: test
test: test/test_dynamic_grid test/test_stack
	./test/test_dynamic_grid
	./test/test_stack

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o test/*.o
