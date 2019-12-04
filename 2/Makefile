.PHONY: all
all: interpeter

%.o: %.c
	gcc -c $< -o $@

interpeter: interpreter.o 1202.o
	gcc $^ -o $@

.PHONY: clean
clean:
	rm -f 2 *.o
