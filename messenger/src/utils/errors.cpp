#include <cstdio>
#include <errors.h>

void check_error(int n, char *str) {
    if (n < 0) {
        perror(str);
        exit(1);
    }
}