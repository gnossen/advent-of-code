.PHONY: all
all: interpreter search

%.o: %.c
	gcc -c $< -o $@

interpreter: interpreter.o 1202.o
	gcc $^ -o $@

search: search.o 1202.o
	gcc $^ -o $@

.PHONY: clean
clean:
	rm -f 2 *.o
