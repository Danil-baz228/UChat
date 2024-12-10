#include "libmx.h"

int mx_quicksort(char **array, int start, int end) {
    if (!array) {
        return -1;
    }

    int swaps = 0;

    if (start < end) {
        int left = start;
        int right = end;
        char *pivot = array[(left + right) / 2];

        while (left <= right) {
            
            while (mx_strlen(array[left]) < mx_strlen(pivot)) {
                left++;
            }
          
            while (mx_strlen(array[right]) > mx_strlen(pivot)) {
                right--;
            }

            
            if (left <= right) {
                if (mx_strlen(array[left]) != mx_strlen(array[right])) {
                    char *temp = array[left];
                    array[left] = array[right];
                    array[right] = temp;
                    swaps++;
                }
                left++;
                right--;
            }
        }

        
        swaps += mx_quicksort(array, start, right);
        swaps += mx_quicksort(array, left, end);
    }
    
    return swaps;
}

