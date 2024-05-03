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
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/sysctl.h>

// Function to format bytes into human-readable format
void formatBytes(unsigned long long bytes, char *buffer, int bufferSize, int human, int si, int unit) {
    const char *suffixesHuman[] = {"B", "Ki", "Mi", "Gi", "Ti", "Pi"};
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int suffixIndex = 0;
    double result = bytes;

    if (human) {
        while (result > 1024 && suffixIndex < 6) {
            result /= 1024.0;
            suffixIndex++;
        }
        if (snprintf(buffer, bufferSize, "%.2f %s", result, suffixesHuman[suffixIndex]) >= bufferSize) {
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
        if (snprintf(buffer, bufferSize, "%.2f %s", result, suffixes[suffixIndex]) >= bufferSize) {
            fprintf(stderr, "Error: Buffer size too small for formatting bytes\n");
        }
    }
}

// Main function
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

    mach_port_t host_port = mach_host_self();
    vm_size_t page_size;
    kern_return_t kr;

    kr = host_page_size(host_port, &page_size);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get page size, code %d\n", kr);
        return 1;
    }
        
    // Fetch total physical memory
    host_basic_info_data_t hostInfo = {0};
    mach_msg_type_number_t info_count = HOST_BASIC_INFO_COUNT;
    kr = host_info(host_port, HOST_BASIC_INFO, (host_info_t)&hostInfo, &info_count);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get total memory, code %d\n", kr);
        return 1;
    }

    // Fetch detailed memory statistics
    vm_statistics64_data_t vm_stat = {0};
    mach_msg_type_number_t host_size = sizeof(vm_statistics64_data_t) / sizeof(integer_t);
    kr = host_statistics64(host_port, HOST_VM_INFO64, (host_info_t)&vm_stat, &host_size);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get memory statistics, code %d\n", kr);
        return 1;
    }

    // Calculate memory components
    unsigned long long total_memory = hostInfo.max_mem;
    unsigned long long free_memory = (unsigned long long)(vm_stat.free_count - vm_stat.speculative_count) * page_size;
    unsigned long long wired_memory = (unsigned long long)vm_stat.wire_count * page_size;
    unsigned long long app_memory = (unsigned long long)(vm_stat.internal_page_count - vm_stat.purgeable_count) * page_size;
    unsigned long long cached_memory = (unsigned long long)(vm_stat.purgeable_count + vm_stat.external_page_count) * page_size;
    unsigned long long used_memory = total_memory - free_memory - cached_memory;

    // Get swap information
    struct xsw_usage swapinfo;
    size_t swapinfo_sz = sizeof(swapinfo);
    int mib[2] = {CTL_VM, VM_SWAPUSAGE};
    if (sysctl(mib, 2, &swapinfo, &swapinfo_sz, NULL, 0) != 0) {
        perror("Error: sysctl failed to get swap usage");
        return 1;
    }

    // Get memory commit limit and amount of committed memory
    int mib_commit[2] = {CTL_VM, VM_SWAPUSAGE};
    struct xsw_usage commitinfo;
    size_t commitinfo_sz = sizeof(commitinfo);
    if (sysctl(mib_commit, 2, &commitinfo, &commitinfo_sz, NULL, 0) != 0) {
        perror("Error: sysctl failed to get memory commit info");
        return 1;
    }

    unsigned long long commit_limit = commitinfo.xsu_total;
    unsigned long long committed_memory = commitinfo.xsu_used;
    unsigned long long uncommitted_memory = commitinfo.xsu_avail;

    // Formatting memory sizes
    char totalStr[20], usedStr[20], freeStr[20], cachedStr[20], appStr[20], wiredStr[20];
    char swapTotalStr[20], swapUsedStr[20], swapFreeStr[20];
    char commitLimitStr[20], committedStr[20], uncommittedStr[20];

    formatBytes(total_memory, totalStr, sizeof(totalStr), human, si, unit); // total memory
    formatBytes(used_memory, usedStr, sizeof(usedStr), human, si, unit); // used memory
    formatBytes(free_memory, freeStr, sizeof(freeStr), human, si, unit); // free memory
    formatBytes(cached_memory, cachedStr, sizeof(cachedStr), human, si, unit); // cached memory
    formatBytes(app_memory, appStr, sizeof(appStr), human, si, unit); // app memory
    formatBytes(wired_memory, wiredStr, sizeof(wiredStr), human, si, unit); // wired memory
    formatBytes(swapinfo.xsu_total, swapTotalStr, sizeof(swapTotalStr), human, si, unit); // total swap
    formatBytes(swapinfo.xsu_used, swapUsedStr, sizeof(swapUsedStr), human, si, unit); // used swap
    formatBytes(swapinfo.xsu_avail, swapFreeStr, sizeof(swapFreeStr), human, si, unit); // free swap
    formatBytes(commit_limit, commitLimitStr, sizeof(commitLimitStr), human, si, unit); // memory commit limit
    formatBytes(committed_memory, committedStr, sizeof(committedStr), human, si, unit); // committed memory
    formatBytes(uncommitted_memory, uncommittedStr, sizeof(uncommittedStr), human, si, unit); // uncommitted memory

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
                unsigned long long mem_total = total_memory;
                unsigned long long swap_total = swapinfo.xsu_total;
                unsigned long long total_total = mem_total + swap_total;

                char memTotalStr[20], swapTotalStr[20], totalTotalStr[20];
                formatBytes(mem_total, memTotalStr, sizeof(memTotalStr), human, si, unit);
                formatBytes(swap_total, swapTotalStr, sizeof(swapTotalStr), human, si, unit);
                formatBytes(total_total, totalTotalStr, sizeof(totalTotalStr), human, si, unit);

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