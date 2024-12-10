#include "libmx.h"

void *mx_memchr(const void *s, int c, size_t n) {
    const unsigned char *ptr = s;
    unsigned char value = (unsigned char)c;
    
    for (size_t i = 0; i < n; i++) {
        if (ptr[i] == value) {
            return (void *)(ptr + i);
        }
    }
    
    return NULL;
}

