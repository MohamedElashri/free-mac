/*
 * Copyright (c) 2024 Mohamed Elashri
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include "format_utils.h"

void format_bytes(unsigned long long bytes, char *buffer, int buffer_size, int human, int si, int unit) {
    const char *suffixesHuman[] = {"B", "Ki", "Mi", "Gi", "Ti", "Pi"};
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int suffixIndex = 0;
    double result = bytes;

    if (human) {
        while (result > 1024 && suffixIndex < 6) {
            result /= 1024.0;
            suffixIndex++;
        }
        if (snprintf(buffer, buffer_size, "%.2f %s", result, suffixesHuman[suffixIndex]) >= buffer_size) {
            fprintf(stderr, "Error: Buffer size too small for formatting bytes with suffix\n");
        }
    } else {
        if (si) {
            while (result > 1000 && suffixIndex < unit) {
                result /= 1000.0;
                suffixIndex++;
            }
        } else {
            while (result > 1024 && suffixIndex < unit) {
                result /= 1024.0;
                suffixIndex++;
            }
        }
        if (snprintf(buffer, buffer_size, "%.2f %s", result, suffixes[suffixIndex]) >= buffer_size) {
            fprintf(stderr, "Error: Buffer size too small for formatting bytes\n");
        }
    }
}