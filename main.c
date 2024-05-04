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
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "memory_info.h"
#include "format_utils.h"

int main(int argc, char *argv[]) {
    int human = 0;
    int si = 0;
    int unit = 1;  // Default unit is kibibytes (KiB)
    int wide = 0;
    int count = 1;
    int lohi = 0;
    int line = 0;
    float delay = 1.0;
    int total = 0;
    int committed = 0;

    // Parse command-line options
    static struct option long_options[] = {
        {"bytes", no_argument, 0, 'b'},
        {"kibi", no_argument, 0, 'k'},
        {"mebi", no_argument, 0, 'm'},
        {"gibi", no_argument, 0, 'g'},
        {"tebi", no_argument, 0, 'e'},
        {"pebi", no_argument, 0, 'p'},
        {"kilo", no_argument, 0, 'K'},
        {"mega", no_argument, 0, 'M'},
        {"giga", no_argument, 0, 'G'},
        {"tera", no_argument, 0, 'T'},
        {"peta", no_argument, 0, 'P'},
        {"human", no_argument, 0, 'h'},
        {"wide", no_argument, 0, 'w'},
        {"count", required_argument, 0, 'c'},
        {"lohi", no_argument, 0, 'l'},
        {"line", no_argument, 0, 'L'},
        {"seconds", required_argument, 0, 's'},
        {"si", no_argument, 0, 'S'},
        {"total", no_argument, 0, 't'},
        {"committed", no_argument, 0, 'v'},
        {"help", no_argument, 0, '?'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "bkmgepKMGTPhwc:lLs:StV?", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'b': unit = 0; break;
            case 'k': unit = 1; break;
            case 'm': unit = 2; break;
            case 'g': unit = 3; break;
            case 'e': unit = 4; break;
            case 'p': unit = 5; break;
            case 'K': si = 1; unit = 1; break;
            case 'M': si = 1; unit = 2; break;
            case 'G': si = 1; unit = 3; break;
            case 'T': si = 1; unit = 4; break;
            case 'P': si = 1; unit = 5; break;
            case 'h': human = 1; break;
            case 'w': wide = 1; break;
            case 'c': count = atoi(optarg); break;
            case 'l': lohi = 1; break;
            case 'L': line = 1; break;
            case 's': delay = atof(optarg); break;
            case 'S': si = 1; break;
            case 't': total = 1; break;
            case 'v': committed = 1; break;
            case '?': 
                printf("Usage: %s [OPTIONS]\n", argv[0]);
                printf("Options:\n");
                printf("  -b, --bytes         Display the amount of memory in bytes.\n");
                printf("  -k, --kibi          Display the amount of memory in kibibytes. This is the default.\n");
                printf("  -m, --mebi          Display the amount of memory in mebibytes.\n");
                printf("  -g, --gibi          Display the amount of memory in gibibytes.\n");
                printf("  --tebi              Display the amount of memory in tebibytes.\n");
                printf("  --pebi              Display the amount of memory in pebibytes.\n");
                printf("  --kilo              Display the amount of memory in kilobytes. Implies --si.\n");
                printf("  --mega              Display the amount of memory in megabytes. Implies --si.\n");
                printf("  --giga              Display the amount of memory in gigabytes. Implies --si.\n");
                printf("  --tera              Display the amount of memory in terabytes. Implies --si.\n");
                printf("  --peta              Display the amount of memory in petabytes. Implies --si.\n");
                printf("  -h, --human         Show all output fields automatically scaled to shortest three digit unit and display the units of print out.\n");
                printf("  -w, --wide          Switch to the wide mode. The wide mode produces lines longer than 80 characters.\n");
                printf("  -c, --count count   Display the result count times. Requires the -s option.\n");
                printf("  -l, --lohi          Show detailed low and high memory statistics.\n");
                printf("  -L, --line          Show output on a single line, often used with the -s option to show memory statistics repeatedly.\n");
                printf("  -s, --seconds delay Continuously display the result delay seconds apart.\n");
                printf("  --si                Use kilo, mega, giga etc (power of 1000) instead of kibi, mebi, gibi (power of 1024).\n");
                printf("  -t, --total         Display a line showing the column totals.\n");
                printf("  -v, --committed     Display a line showing the memory commit limit and amount of committed/uncommitted memory.\n");
                printf("  --help              Print help.\n");
                printf("  -V, --version       Display version information.\n");
                exit(0);
            case 'V':
                printf("0.2.0\n");
                exit(0);
            default:
                break;
        }
    }

    MemoryInfo info = get_memory_info();

    // Formatting memory sizes
    char totalStr[20], usedStr[20], freeStr[20], cachedStr[20], appStr[20], wiredStr[20];
    char swapTotalStr[20], swapUsedStr[20], swapFreeStr[20];
    char commitLimitStr[20], committedStr[20], uncommittedStr[20];

    format_bytes(info.total_memory, totalStr, sizeof(totalStr), human, si, unit);
    format_bytes(info.used_memory, usedStr, sizeof(usedStr), human, si, unit);
    format_bytes(info.free_memory, freeStr, sizeof(freeStr), human, si, unit);
    format_bytes(info.cached_memory, cachedStr, sizeof(cachedStr), human, si, unit);
    format_bytes(info.app_memory, appStr, sizeof(appStr), human, si, unit);
    format_bytes(info.wired_memory, wiredStr, sizeof(wiredStr), human, si, unit);
    format_bytes(info.swapinfo.xsu_total, swapTotalStr, sizeof(swapTotalStr), human, si, unit);
    format_bytes(info.swapinfo.xsu_used, swapUsedStr, sizeof(swapUsedStr), human, si, unit);
    format_bytes(info.swapinfo.xsu_avail, swapFreeStr, sizeof(swapFreeStr), human, si, unit);
    format_bytes(info.commit_limit, commitLimitStr, sizeof(commitLimitStr), human, si, unit);
    format_bytes(info.committed_memory, committedStr, sizeof(committedStr), human, si, unit);
    format_bytes(info.uncommitted_memory, uncommittedStr, sizeof(uncommittedStr), human, si, unit);

    // Printing results
    for (int i = 0; i < count; i++) {
        if (line) {
            printf("Mem: %s total, %s used, %s free, %s cached, %s app, %s wired", totalStr, usedStr, freeStr, cachedStr, appStr, wiredStr);
            printf("Swap: %s total, %s used, %s free", swapTotalStr, swapUsedStr, swapFreeStr);
            printf("\n");
        } else {
            if (wide) {
                printf("%15s %15s %15s %15s %15s %15s %15s\n", "total", "used", "free", "buff/cache", "available", "app", "wired");
                printf("Mem: %15s %15s %15s %15s %15s %15s %15s\n", totalStr, usedStr, freeStr, cachedStr, freeStr, appStr, wiredStr);
                printf("Swap: %14s %14s %14s\n", swapTotalStr, swapUsedStr, swapFreeStr);
            } else {
                printf("%15s %15s %15s %15s %15s %15s\n", "total", "used", "free", "cached", "app", "wired");
                printf("Mem: %15s %15s %15s %15s %15s %15s\n", totalStr, usedStr, freeStr, cachedStr, appStr, wiredStr);
                printf("Swap: %14s %14s %14s\n", swapTotalStr, swapUsedStr, swapFreeStr);
            }

            if (lohi) {
                // TODO: Implement low and high memory statistics
            }

            if (total) {
                unsigned long long mem_total = info.total_memory;
                unsigned long long swap_total = info.swapinfo.xsu_total;
                unsigned long long total_total = mem_total + swap_total;

                char memTotalStr[20], swapTotalStr[20], totalTotalStr[20];
                format_bytes(mem_total, memTotalStr, sizeof(memTotalStr), human, si, unit);
                format_bytes(swap_total, swapTotalStr, sizeof(swapTotalStr), human, si, unit);
                format_bytes(total_total, totalTotalStr, sizeof(totalTotalStr), human, si, unit);

                printf("Total: %13s %13s %13s\n", memTotalStr, swapTotalStr, totalTotalStr);
            }

            if (committed) {
                printf("Commit Limit: %15s\n", commitLimitStr);
                printf("Committed: %18s\n", committedStr);
                printf("Uncommitted: %16s\n", uncommittedStr);
            }
        }

        if (i < count - 1) {
            sleep(delay);
        }
    }

    return 0;
}