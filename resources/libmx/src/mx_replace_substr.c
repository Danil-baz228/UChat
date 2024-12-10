#include "libmx.h"

char *mx_replace_substr(const char *str, const char *sub, const char *replace) {
    if (str == NULL || sub == NULL || replace == NULL) {
        return NULL;
    }

    int str_len = mx_strlen(str);
    int sub_len = mx_strlen(sub);
    int replace_len = mx_strlen(replace);

    int occurrence_count = 0;
    const char *search_pos = str;
    while ((search_pos = mx_strstr(search_pos, sub)) != NULL) {
        occurrence_count++;
        search_pos += sub_len;
    }

    if (occurrence_count == 0) {
        return mx_strdup(str);
    }

    int new_length = str_len + (replace_len - sub_len) * occurrence_count;
    char *result = (char *)malloc(new_length + 1);
    if (result == NULL) {
        return NULL;
    }

    char *result_ptr = result;
    const char *str_ptr = str;

    while (*str_ptr) {
        if (mx_strstr(str_ptr, sub) == str_ptr) {
            mx_strcpy(result_ptr, replace);
            result_ptr += replace_len;
            str_ptr += sub_len;
        } else {
            *result_ptr++ = *str_ptr++;
        }
    }

    *result_ptr = '\0';

    return result;
}

