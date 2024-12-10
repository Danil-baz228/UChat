#include "libmx.h"

int mx_bubble_sort(char **arr, int size) {
    int swaps = 0;
    int i, j;
    char *temp;

    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (mx_strcmp(arr[j], arr[j + 1]) > 0) {
                
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                
                swaps++;
            }
        }
    }

    return swaps;
}

