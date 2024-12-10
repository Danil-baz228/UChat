#include "libmx.h"

void *mx_memrchr(const void *s, int c, size_t n) {
    const unsigned char *ptr = s;
    unsigned char value = (unsigned char)c;


    for (size_t i = n; i > 0; i--) {
        if (ptr[i - 1] == value) {
            return (void *)(ptr + i - 1);
        }
    }
    
    return NULL;
}
