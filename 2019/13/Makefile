EXECUTABLES=arcade

.PHONY: all
all: ${EXECUTABLES}

CFLAGS=-I../5/

%.o: %.c
	gcc "${CFLAGS}" -c $< -o $@

arcade: arcade.o ../5/1202.o
	gcc "${CFLAGS}" $^ -o $@

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o
