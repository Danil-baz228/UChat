#include "libmx.h"


char *mx_itoa(int number) {

    if (number == 0) {
        char *str = (char *)malloc(2); 
        if (str == NULL) return NULL; 
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    
    int num = number;
    int length = 0;
    if (num < 0) {
        length++; 
        num = -num;
    }
    
    int temp = num;
    do {
        length++;
        temp /= 10;
    } while (temp > 0);

   
    char *str = (char *)malloc(length + 1); 
    if (str == NULL) return NULL;
    
    str[length] = '\0'; 

    
    num = number;
    if (num < 0) {
        str[0] = '-';
        num = -num;
    }

    while (num > 0) {
        str[--length] = (num % 10) + '0';
        num /= 10;
    }

    return str;
}

