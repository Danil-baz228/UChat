#include "libmx.h"

char *mx_strndup(const char *s1, size_t n) {
    
    size_t len = mx_strlen(s1);
    size_t copy_len = (n < len) ? n : len;

    char *s2 = (char *)malloc(copy_len + 1);

    if (s2 == NULL) {
        return NULL;
    }

    mx_strncpy(s2, s1, copy_len);

    s2[copy_len] = '\0';

    return s2;
}

