#include "libmx.h"

int mx_get_substr_index(const char *str, const char *sub) {
    if (str == NULL || sub == NULL) {
        return -2;
    }
    
    char *pos = mx_strstr(str, sub);
    
    if (pos == NULL) {
        return -1;
    }
    
    return pos - str;
}
