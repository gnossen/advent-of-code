EXECUTABLES=interpreter pretty_print test/buffer_test test/bytecode_test

.PHONY: all
all: ${EXECUTABLES}

%.o: %.c
	gcc -c $< -o $@

interpreter: interpreter.o 1202.o
	gcc $^ -o $@

pretty_print: pretty_print.o 1202.o
	gcc $^ -o $@

test/buffer_test: test/buffer_test.o 1202.o
	gcc $^ -o $@

test/bytecode_test: test/bytecode_test.o 1202.o
	gcc $^ -o $@

.PHONY: test
test: test/buffer_test test/bytecode_test pretty_print interpreter
	./test/test.sh

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o
