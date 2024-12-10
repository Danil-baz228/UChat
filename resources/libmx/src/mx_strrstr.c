#include "libmx.h"

char *mx_strrstr(const char *haystack, const char *needle) {
    if (*needle == '\0') {
        return (char *)haystack;
    }

    char *last_occurrence = NULL;
    while (*haystack) {
        if (mx_strncmp(haystack, needle, mx_strlen(needle)) == 0) {
            last_occurrence = (char *)haystack;
        }
        haystack++;
    }
    return last_occurrence;
}
