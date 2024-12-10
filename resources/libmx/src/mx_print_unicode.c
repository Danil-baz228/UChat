#include "libmx.h"

void mx_print_unicode(wchar_t c)
{
    if (c < 0x80) {
        mx_printchar(c);
    }
    else if (c < 0x0800) {
        mx_printchar((c >> 6) + 0xC0);
        mx_printchar((c & 0x3F) + 0x80);
    }
    else if (c < 0x010000) {
        mx_printchar((c >> 12) + 0xE0);
        mx_printchar(((c >> 6) & 0x3F) + 0x80);
        mx_printchar((c & 0x3F) + 0x80);
    }
    else if (c < 0x110000) {
        mx_printchar((c >> 18) + 0xF0);
        mx_printchar(((c >> 12) & 0x3F) + 0x80);
        mx_printchar(((c >> 6) & 0x3F) + 0x80);
        mx_printchar((c & 0x3F) + 0x80);
    }
}
