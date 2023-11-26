EXECUTABLES=camera test/test_grid

.PHONY: all
all: ${EXECUTABLES}

CC=g++
CFLAGS=-I../5 -g -fsanitize=address
CCFLAGS=${CFLAGS}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.cc
	${CC} ${CCFLAGS} -c $< -o $@

camera: camera.o ../5/1202pp.o ../5/1202.o
	${CC} ${CCFLAGS} $^ -o $@

test/test_grid: test/test_grid.o grid.o
	${CC} ${CCFLAGS} $^ -o $@

.PHONY: test
test: test/test_grid
	./test/test_grid

.PHONY: clean
clean:
	rm -f ${EXECUTABLES} *.o test/*.o
	rm -f ../5/*.o
