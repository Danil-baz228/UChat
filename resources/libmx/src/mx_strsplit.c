#include "libmx.h"

char **mx_strsplit(const char *s, char c) {
    if (s == NULL)
    {
        return NULL;
    } 
    
    int amount = mx_count_words(s,c);
    char **result = (char **)malloc((amount + 1) * sizeof(char *));
    int i = 0, count = 0;
    for (int k = 0; k < mx_strlen(s) + 1; k++)
    {
        if ((s[k] == c || s[k] == '\0') && count > 0)
        {
            result[i] = mx_strnew(count);
            mx_strncpy(result[i], &s[k-count], count);
            i++;
            count = 0;
        }
        else if (s[k] != c)
        {
            count++;
        }
    }
    
    result[amount] = NULL;
    return result;
}

