#include "libmx.h"

int mx_count_words(const char *str, char c) {
    if (str == NULL) {
        return -1;
    }

    int count = 0;
    bool in_word = false;

    while (*str) {
        if (*str != c) {
            if (!in_word) {
                count++;
                in_word = true;
            }
        } else {
            in_word = false;
        }
        str++;
    }

    return count;
}

