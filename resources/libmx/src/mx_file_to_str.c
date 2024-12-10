#include "libmx.h"

char *mx_file_to_str(const char *file) {
    if (file == NULL) {
        return NULL;
    }

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        return NULL; 
    }

  
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        close(fd);
        return NULL; 
    }
    lseek(fd, 0, SEEK_SET); 

   
    char *content = (char *)malloc((size_t)file_size + 1);
    if (content == NULL) {
        close(fd);
        return NULL; 
    }

    ssize_t bytes_read = read(fd, content, (size_t)file_size);
    if (bytes_read != (ssize_t)file_size) {
        free(content);
        close(fd);
        return NULL; 
    }

    content[bytes_read] = '\0'; 
    close(fd);
    return content;
}

