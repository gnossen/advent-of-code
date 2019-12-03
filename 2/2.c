#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

const uint64_t k_add_op = 1;
const uint64_t k_mult_op = 2;
const uint64_t k_halt_op = 99;

#define MIN(a, b) ((a > b) ? b : a)

static uint64_t initial[] = {
    1, 9, 10, 3,
    2, 3, 11, 0,
    99,
    30, 40, 50
};

void advance(const uint64_t * start, const uint64_t ** ip, size_t steps, size_t prog_len) {
    if ((*ip - start) + steps > prog_len + 1) {
        fprintf(stderr, "Malformed program. Attempt to advance to position %d when buffer is only of size %d.\n", (*ip - start) + steps, prog_len);
        exit(1);
    }
    (*ip) += steps;
}

void pretty_print_program(int fd, const uint64_t* program, size_t len) {
    const uint64_t * ip = program;
    do {
        if (*ip == k_halt_op) {
            advance(program, &ip, 1, len);
            dprintf(fd, "%d\n", *(ip - 1));
        } else if (*ip == k_add_op || *ip == k_mult_op){
            advance(program, &ip, 4, len);
            dprintf(fd, "%d, %d, %d, %d\n", *(ip - 4), *(ip - 3), *(ip - 2), *(ip - 1));
        } else {
            size_t upper = MIN(4, len - (ip - program));
            for (size_t i = 0; i < upper; ++i) {
                advance(program, &ip, 1, len);
                dprintf(fd, "%d", *(ip - 1));
                if (ip != program + len) {
                    dprintf(fd, ", ");
                }

            }
            dprintf(fd, "\n");
        }
    } while(ip < program + len);
}

int main(int argc, char ** argv) {
    pretty_print_program(STDOUT_FILENO, initial, sizeof(initial) / sizeof(uint64_t));
    return 0;
}
