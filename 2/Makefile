2.o: 2.c
	gcc -c $< -o $@

1202.o: 1202.c
	gcc -c $< -o $@

2: 2.o 1202.o
	gcc $^ -o $@

.PHONY: clean
clean:
	rm -f 2 *.o
