#include "libmx.h"

void mx_del_strarr(char ***arr) {
    if (arr == NULL || *arr == NULL) {
        return; 
    }
    
    char **current = *arr;
    
    while (*current) {
        free(*current);
        current++;
    }
    
    free(*arr);
    *arr = NULL;
}

