#include "libmx.h"

void *mx_memcpy(void *restrict dst, const void *restrict src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dst;
}
