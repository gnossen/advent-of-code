#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PASSWORD_LEN 6

int valid(const char* password, size_t len) {
    int has_repeat = 0;
    for (size_t i = 1; i < len; ++i) {
        if (password[i] == password[i - 1]) {
            has_repeat = 1;
        }
        if (password[i] < password[i - 1]) {
            return 0;
        }
    }
    return has_repeat;
}

void increment(char* number, size_t len) {
    if (len == 0) {
        fprintf(stderr, "Attempt to increment number of length 0.\n");
    }
    if (number[len - 1] == '9') {
        number[len - 1] = '0';
        increment(number, len - 1);
    } else {
        number[len - 1]++;
    }
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s low high\n");
        exit(1);
    }
    for (size_t i = 1; i <= 2; ++i) {
        if (strlen(argv[i]) != PASSWORD_LEN) {
            fprintf(stderr, "%s is not %d digits.\n", argv[i], PASSWORD_LEN);
            exit(1);
        }
    }
    char * start = argv[1];
    char * stop = argv[2];
    increment(stop, PASSWORD_LEN);
    char candidate[PASSWORD_LEN];
    strncpy((char *)&candidate, start, PASSWORD_LEN);
    size_t count = 0;
    while(strncmp(stop, (const char *)&candidate, PASSWORD_LEN)) {
        if (valid((const char *)&candidate, PASSWORD_LEN)) {
            count++;
        }
        increment((char *)&candidate, PASSWORD_LEN);
    }
    printf("%d\n", count);
    return 0;
}
