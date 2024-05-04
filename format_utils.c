#include <stdio.h>
#include "format_utils.h"

void format_bytes(unsigned long long bytes, char *buffer, int buffer_size, int human, int si, int unit) {
    const char *suffixesHuman[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int suffixIndex = 0;
    double result = bytes;

    if (human) {
        while (result >= 1024 && suffixIndex < 5) {
            result /= 1024.0;
            suffixIndex++;
        }
    } else {
        while (result >= (si ? 1000 : 1024) && suffixIndex < unit) {
            result /= (si ? 1000.0 : 1024.0);
            suffixIndex++;
        }
    }

    if (snprintf(buffer, buffer_size, "%.2f %s", result, (human ? suffixesHuman : (si ? suffixes : suffixesHuman))[suffixIndex]) >= buffer_size) {
        fprintf(stderr, "Error: Buffer size too small for formatting bytes\n");
    }
}