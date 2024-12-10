#include "libmx.h"

char *mx_strcat(char *restrict s1, const char *restrict s2) {
    size_t len_s1 = mx_strlen(s1);

    size_t len_s2 = mx_strlen(s2);

    char *dst = s1 + len_s1;

    for (size_t i = 0; i <= len_s2; i++) {
        dst[i] = s2[i];
    }

    return s1;
}

