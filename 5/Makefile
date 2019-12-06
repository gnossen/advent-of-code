EXECUTABLES=interpreter pretty_print buffer_test buffer_test bytecode_test

.PHONY: all
all: ${EXECUTABLES}

%.o: %.c
	gcc -c $< -o $@

interpreter: interpreter.o 1202.o
	gcc $^ -o $@

pretty_print: pretty_print.o 1202.o
	gcc $^ -o $@

buffer_test: buffer_test.o 1202.o
	gcc $^ -o $@

bytecode_test: bytecode_test.o 1202.o
	gcc $^ -o $@

.PHONY: test
test: buffer_test bytecode_test pretty_print interpreter
	./test.sh

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o
