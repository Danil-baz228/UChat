#include "libmx.h"

char *mx_strncat(char *dest, const char *src, int n) {
    if (!dest || !src || n <= 0)
        return dest;

    int dest_len = mx_strlen(dest);
    int i = 0;

    while (i < n && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';

    return dest;
}
