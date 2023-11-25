EXECUTABLES=test/test_dynamic_grid

.PHONY: all
all: ${EXECUTABLES}

CFLAGS=-g

%.o: %.c
	gcc ${CFLAGS} -c $< -o $@

test/test_dynamic_grid: test/test_dynamic_grid.o dynamic_grid.o
	gcc ${CFLAGS} $^ -o $@

.PHONY: test
test: test/test_dynamic_grid
	./test/test_dynamic_grid

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o test/*.o
