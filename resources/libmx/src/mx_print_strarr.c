#include "libmx.h"

void mx_print_strarr(char **arr, const char *delim)
{
    if (arr != NULL && delim != NULL)
    {
        int index = 0;
        while (arr[index] != NULL)
        {
            mx_printstr(arr[index]);
            if (arr[index + 1] != NULL)
            {
                mx_printstr(delim);
            }
            index++;
        }
        mx_printchar('\n');
    }
}

