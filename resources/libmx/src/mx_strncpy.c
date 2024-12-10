#include "libmx.h"

char *mx_strncpy(char *dst, const char *src, int len) {
    if (dst == NULL || src == NULL || len <= 0) {

        return NULL;
        
    }

    char *result = dst;

    while (len > 0 && *src != '\0') {

        *dst = *src;
        dst++;
        src++;
        len--;
    }

    while (len > 0) {
        *dst = '\0';
        dst++;
        len--;
    }

    return result;
}

