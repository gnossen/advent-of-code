.PHONY: all
all: interpreter buffer_test

%.o: %.c
	gcc -c $< -o $@

interpreter: interpreter.o 1202.o
	gcc $^ -o $@

buffer_test: buffer_test.o 1202.o
	gcc $^ -o $@

.PHONY: clean
clean:
	rm -f 2 *.o
