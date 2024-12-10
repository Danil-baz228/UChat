#include "libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
    int left = 0;
    int right = size - 1;
    int midel;
    
    if (!*arr)
        return -1;
    else {
        while (left <= right) {
            (*count)++;
            midel = (right + left) / 2;

            if (mx_strcmp(arr[midel], s) == 0)
                return midel;
            else if (mx_strcmp(arr[midel], s) > 0)
                right = midel - 1;
            else
                left = midel + 1;
        }

        *count = 0;
        return -1;
    }
}
