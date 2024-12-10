#include "libmx.h"

char *mx_del_extra_spaces(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    char *trimmed_str = mx_strtrim(str);
    if (trimmed_str == NULL) {
        return NULL;
    }

    size_t len = mx_strlen(trimmed_str);
    char *cleaned_str = (char *)malloc(len + 1);
    if (cleaned_str == NULL) {
        free(trimmed_str);
        return NULL;
    }

    size_t j = 0;
    bool in_space = false;

    for (size_t i = 0; i < len; i++) {
        if (isspace((unsigned char)trimmed_str[i])) {
            if (!in_space) {
                cleaned_str[j++] = ' ';
                in_space = true;
            }
        } else {
            cleaned_str[j++] = trimmed_str[i];
            in_space = false;
        }
    }

    cleaned_str[j] = '\0';


    free(trimmed_str);

    return cleaned_str;
}


