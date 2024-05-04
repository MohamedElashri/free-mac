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
    while ((opt = getopt_long(argc, argv, "bkmgepKMGTPhwc:lLs:StvV?", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'b': unit = 0; si = 0; break;
            case 'k': unit = 1; si = 0; break;
            case 'm': unit = 2; si = 0; break;
            case 'g': unit = 3; si = 0; break;
            case 'e': unit = 4; si = 0; break;
            case 'p': unit = 5; si = 0; break;
            case 'K': unit = 1; si = 1; break;
            case 'M': unit = 2; si = 1; break;
            case 'G': unit = 3; si = 1; break;
            case 'T': unit = 4; si = 1; break;
            case 'P': unit = 5; si = 1; break;
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
    char totalStr[20], usedStr[20], freeStr[20], sharedStr[20], buffCacheStr[20], availableStr[20];
    char swapTotalStr[20], swapUsedStr[20], swapFreeStr[20];
    char commitLimitStr[20], committedStr[20], uncommittedStr[20];

    format_bytes(info.total_memory, totalStr, sizeof(totalStr), human, si, human ? 0 : unit);
    format_bytes(info.used_memory, usedStr, sizeof(usedStr), human, si, human ? 0 : unit);
    format_bytes(info.free_memory, freeStr, sizeof(freeStr), human, si, human ? 0 : unit);
    format_bytes(info.shared_memory, sharedStr, sizeof(sharedStr), human, si, human ? 0 : unit);
    format_bytes(info.cached_memory, buffCacheStr, sizeof(buffCacheStr), human, si, human ? 0 : unit);
    format_bytes(info.available_memory, availableStr, sizeof(availableStr), human, si, human ? 0 : unit);
    format_bytes(info.swapinfo.xsu_total, swapTotalStr, sizeof(swapTotalStr), human, si, human ? 0 : unit);
    format_bytes(info.swapinfo.xsu_used, swapUsedStr, sizeof(swapUsedStr), human, si, human ? 0 : unit);
    format_bytes(info.swapinfo.xsu_avail, swapFreeStr, sizeof(swapFreeStr), human, si, human ? 0 : unit);
    format_bytes(info.commit_limit, commitLimitStr, sizeof(commitLimitStr), human, si, human ? 0 : unit);
    format_bytes(info.committed_memory, committedStr, sizeof(committedStr), human, si, human ? 0 : unit);
    format_bytes(info.uncommitted_memory, uncommittedStr, sizeof(uncommittedStr), human, si, human ? 0 : unit);

// Printing results
for (int i = 0; i < count; i++) {
    if (line) {
        printf("Mem:  %10s %10s %10s %10s %10s %10s", totalStr, usedStr, freeStr, sharedStr, buffCacheStr, availableStr);
        printf("Swap: %10s %10s %10s", swapTotalStr, swapUsedStr, swapFreeStr);
        printf("\n");
    } else {
        if (human) {
            printf("      %10s %10s %10s %10s %10s %10s\n", "total", "used", "free", "shared", "buff/cache", "available");
            printf("Mem:  %10s %10s %10s %10s %10s %10s\n", totalStr, usedStr, freeStr, sharedStr, buffCacheStr, availableStr);
            printf("Swap: %10s %10s %10s\n", swapTotalStr, swapUsedStr, swapFreeStr);
        } else {
            if (wide) {
                printf("      %15s %15s %15s %15s %15s\n", "total", "used", "free", "buff/cache", "available");
                printf("Mem:  %15s %15s %15s %15s %15s\n", totalStr, usedStr, freeStr, buffCacheStr, availableStr);
                printf("Swap: %15s %15s %15s\n", swapTotalStr, swapUsedStr, swapFreeStr);
            } else {
                printf("      %15s %15s %15s %15s %15s\n", "total", "used", "free", "cached", "available");
                printf("Mem:  %15s %15s %15s %15s %15s\n", totalStr, usedStr, freeStr, buffCacheStr, availableStr);
                printf("Swap: %15s %15s %15s\n", swapTotalStr, swapUsedStr, swapFreeStr);
            }
        }

        if (lohi) {
            // TODO: Implement low and high memory statistics
        }

        if (total) {
            unsigned long long mem_total = info.total_memory;
            unsigned long long swap_total = info.swapinfo.xsu_total;
            unsigned long long total_total = mem_total + swap_total;

            char memTotalStr[20], swapTotalStr[20], totalTotalStr[20];
            format_bytes(mem_total, memTotalStr, sizeof(memTotalStr), human, si, human ? 0 : unit);
            format_bytes(swap_total, swapTotalStr, sizeof(swapTotalStr), human, si, human ? 0 : unit);
            format_bytes(total_total, totalTotalStr, sizeof(totalTotalStr), human, si, human ? 0 : unit);

            printf("      %10s %10s %10s\n", "Mem", "Swap", "Total");
            printf("Total %10s %10s %10s\n", memTotalStr, swapTotalStr, totalTotalStr);
        }

        if (committed) {
            printf("Commit Limit:  %10s\n", commitLimitStr);
            printf("Committed:     %10s\n", committedStr);
            printf("Uncommitted:   %10s\n", uncommittedStr);
        }
    }

    if (i < count - 1) {
        sleep(delay);
    }
}
    return 0;
}