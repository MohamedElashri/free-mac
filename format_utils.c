#include <stdio.h>
#include "format_utils.h"

void format_bytes(unsigned long long bytes, char *buffer, int buffer_size, int human, int si, int unit) {
    const char *suffixes[] = {"B", "K", "M", "G", "T", "P"};
    int suffixIndex = 0;
    double result = bytes;

    if (human) {
        while (result >= 1024 && suffixIndex < (sizeof(suffixes) / sizeof(suffixes[0]) - 1)) {
            result /= 1024.0;
            suffixIndex++;
        }
        if (snprintf(buffer, buffer_size, "%.0f%s", result, suffixes[suffixIndex]) >= buffer_size) {
            fprintf(stderr, "Error: Buffer size too small for formatting bytes\n");
        }
    } else {
        while (result >= 1024 && suffixIndex < unit) {
            result /= (si ? 1000.0 : 1024.0);
            suffixIndex++;
        }
        if (snprintf(buffer, buffer_size, "%.2f %s", result, (si ? suffixes : suffixes)[suffixIndex]) >= buffer_size) {
            fprintf(stderr, "Error: Buffer size too small for formatting bytes\n");
        }
    }
}