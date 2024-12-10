#include "libmx.h"

void *mx_realloc(void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        return malloc(size);
    }
    
    
    void *new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL; 
    }
    
    
    size_t old_size = *((size_t*)((char*)ptr - sizeof(size_t)));

   
    mx_memcpy(new_ptr, ptr, old_size < size ? old_size : size);
    
    
    free(ptr);
    
    return new_ptr;
}
