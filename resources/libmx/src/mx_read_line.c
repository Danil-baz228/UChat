#include "libmx.h"

int mx_read_line(char **lineptr, size_t buf_size, char delim, int fd) {
    if (lineptr == NULL || buf_size == 0 || fd < 0) {
        return -2; 
    }

    char *buffer = mx_strnew(buf_size);
    if (buffer == NULL) {
        return -2; 
    }

    size_t length = 0;
    ssize_t bytes_read;
    char ch;

    while (true) {
        bytes_read = read(fd, &ch, 1);
        if (bytes_read == -1) {
            mx_strdel(&buffer);
            return -2; 
        } else if (bytes_read == 0 && length == 0) {
            mx_strdel(&buffer);
            return -1; 
        } else if (bytes_read == 0 || ch == delim) {
            break; 
        }

        
        if (length >= buf_size - 1) {
            buf_size *= 2;
            char *new_buffer = mx_strnew(buf_size);
            if (new_buffer == NULL) {
                mx_strdel(&buffer);
                return -2;
            }
            mx_memcpy(new_buffer, buffer, length);
            mx_strdel(&buffer);
            buffer = new_buffer;
        }

        buffer[length++] = ch;
    }

    buffer[length] = '\0';
    *lineptr = buffer;
    
    return (int)length;
}

