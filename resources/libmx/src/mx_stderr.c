#include "libmx.h"

void mx_stderr(const char *s) {
    if (s)
        write(STDERR_FILENO, s, mx_strlen(s));
}
