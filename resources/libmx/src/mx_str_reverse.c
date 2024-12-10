#include "libmx.h"

void mx_str_reverse(char *s) {
    if (s == NULL) {
        return;
    }


    char *start = s;
    char *end = s + mx_strlen(s) - 1;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}

