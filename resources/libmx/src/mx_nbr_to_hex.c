#include "libmx.h"


char* mx_nbr_to_hex(unsigned long number) {
    unsigned long temp = number;
    int hex_length = 0;
    
    
    while (temp) {
        temp /= 16;
        hex_length++;
    }

    
    char* hex_result = (char*)malloc((hex_length + 1) * sizeof(char));
    if (!hex_result) {
        return NULL;
    }

    
    for (int i = hex_length - 1; i >= 0; --i) {
        unsigned int digit = number % 16;
        hex_result[i] = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
        number /= 16;
    }

    hex_result[hex_length] = '\0';

    return hex_result;
}

