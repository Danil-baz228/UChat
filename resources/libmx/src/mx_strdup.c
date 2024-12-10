#include "libmx.h"

char *mx_strdup(const char *s1) {
    size_t len = mx_strlen(s1) + 1;


    char *s2 = (char *)malloc(len);

    if (s2 == NULL) {
        return NULL;
    }

    mx_strcpy(s2, s1);

    return s2;
}

