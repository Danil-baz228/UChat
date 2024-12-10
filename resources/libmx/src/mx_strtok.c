#include "libmx.h"

char *mx_strtok(char *str, const char *delim) {
    static char *last = NULL; 
    if (str == NULL && last == NULL) {
        return NULL;
    }

    if (str != NULL) {
        last = str;
    }

   
    last += strspn(last, delim);
    if (*last == '\0') {
        return NULL; 
    }

    char *token = last;
    
    last = strpbrk(last, delim);
    if (last != NULL) {
        *last = '\0'; 
        last++; 
    }

    return token;
}
