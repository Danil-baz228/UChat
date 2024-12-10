#include "libmx.h"


unsigned long mx_hex_to_nbr(const char *hex) {
    if (hex == NULL) {
        return 0;
    }

    unsigned long result = 0;
    while (*hex) {
        char c = *hex;
        unsigned long value;

        if (isdigit(c)) {
            value = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else {
            return 0;  
        }

        result = result * 16 + value;  
        hex++;
    }

    return result;
}


