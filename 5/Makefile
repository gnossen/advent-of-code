.PHONY: all
all: interpreter pretty_print buffer_test

%.o: %.c
	gcc -c $< -o $@

interpreter: interpreter.o 1202.o
	gcc $^ -o $@

pretty_print: pretty_print.o 1202.o
	gcc $^ -o $@

buffer_test: buffer_test.o 1202.o
	gcc $^ -o $@

.PHONY: test
test: buffer_test pretty_print interpreter
	./test.sh

.PHONY: clean
clean:
	rm -f 2 *.o
