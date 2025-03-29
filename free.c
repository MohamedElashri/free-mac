/*
 * Copyright (c) 2025 Mohamed Elashri
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
 #include <string.h>
 #include <getopt.h>
 #include <mach/mach.h>
 #include <sys/types.h>
 #include <sys/sysctl.h>
 #include <signal.h>
 #include <errno.h>
 #include <limits.h>
 #include <stdarg.h>
 
 /* Constants */
 #define PROGRAM_VERSION "0.3"
 #define MEMORY_STRING_BUFFER_SIZE 32
 #define MAX_COUNT_VALUE 1000
 #define MAX_DELAY_VALUE 3600.0  /* Maximum delay of 1 hour */
 #define MIN_DELAY_VALUE 0.1     /* Minimum delay of 0.1 seconds */
 #define MAX_EXPECTED_MEMORY (1ULL * 1024 * 1024 * 1024 * 1024) /* 1 TiB limit */
 
 /* Global variables */
 mach_port_t g_host_port = MACH_PORT_NULL;
 
 /* Log levels for error reporting */
 typedef enum {
     DEBUG,
     INFO,
     WARNING,
     ERROR,
     FATAL
 } LogLevel;
 
 /* Default log level - can be changed via command line arguments */
 LogLevel current_log_level = WARNING;
 
 /* Cleanup macro to ensure consistent resource management */
 #define CLEANUP_AND_EXIT(code) do { cleanup(); return code; } while(0)
 
 /* Function prototypes */
 void cleanup(void);
 void signal_handler(int signum);
 void log_message(LogLevel level, const char *format, ...);
 int formatBytes(unsigned long long bytes, char *buffer, int bufferSize, int human, int si, int unit);
 void print_usage(const char *program_name);
 void print_version(void);
 
 /**
  * Cleanup function to release all resources
  * Called before exit or on error conditions
  */
 void cleanup(void) {
     /* Deallocate Mach port if it's valid */
     if (g_host_port != MACH_PORT_NULL) {
         kern_return_t kr = mach_port_deallocate(mach_task_self(), g_host_port);
         if (kr != KERN_SUCCESS) {
             log_message(WARNING, "Warning: Failed to deallocate host port, code %d\n", kr);
         }
         g_host_port = MACH_PORT_NULL;
     }

     
 }
 
 /**
  * Signal handler for graceful termination
  * Catches signals like SIGINT and SIGTERM
  */
 void signal_handler(int signum) {
     log_message(INFO, "\nReceived signal %d, cleaning up...\n", signum);
     cleanup();
     exit(signum);
 }
 
 /**
  * Logging function with support for different log levels
  * Only logs messages at or above the current log level
  */
 /**
  * Logging function with support for different log levels
  * Only logs messages at or above the current log level
  * Format follows Linux utilities convention - program name: error message
  */
 void log_message(LogLevel level, const char *format, ...) {
     if (level < current_log_level) return;
     
     va_list args;
     va_start(args, format);
     
     /* Use stderr for errors, stdout for debug/info when explicitly requested */
     FILE *stream = (level >= ERROR) ? stderr : stdout;
     
     /* Add prefix only for error messages in Linux style */
     if (level >= ERROR) {
         fprintf(stream, "free: ");
     } else if (level == DEBUG) {
         /* Only add [DEBUG] prefix in debug mode */
         fprintf(stream, "[DEBUG] ");
     }
     /* No prefix for INFO/WARNING in normal output - keeps display clean */
     
     vfprintf(stream, format, args);
     va_end(args);
     
     /* Ensure fatal errors exit after logging */
     if (level == FATAL) {
         cleanup();
         exit(EXIT_FAILURE);
     }
 }
 
 /**
  * Formats byte values into human-readable strings with appropriate units
  * Returns 0 on success, -1 on error
  */
 int formatBytes(unsigned long long bytes, char *buffer, int bufferSize, int human, int si, int unit) {
     if (buffer == NULL || bufferSize <= 0) {
         return -1;
     }
     
     /* Define unit suffixes */
     const char *suffixesHuman[] = {"B", "Ki", "Mi", "Gi", "Ti", "Pi"};
     const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
     
     /* Validate unit parameter */
     if (unit < 0 || unit > 5) {
         unit = 0; /* Default to bytes on invalid input */
     }
     
     int suffixIndex = 0;
     double result = (double)bytes;
     
     /* Calculate value based on unit parameters */
     if (human) {
         /* Guard against division by zero */
         unsigned long divisor = si ? 1000 : 1024;
         
         while (result > divisor && suffixIndex < 5) {
             result /= divisor;
             suffixIndex++;
         }
         
         /* Format and check for truncation */
         int ret = snprintf(buffer, bufferSize, "%.2f %s", result, suffixesHuman[suffixIndex]);
         if (ret < 0 || ret >= bufferSize) {
             /* Handle truncation safely */
             strncpy(buffer, "ERROR", bufferSize - 1);
             buffer[bufferSize - 1] = '\0';
             return -1;
         }
     } else {
         /* Apply unit conversion */
         unsigned long divisor = si ? 1000 : 1024;
         
         /* Apply unit conversion up to specified unit */
         for (int i = 0; i < unit && suffixIndex < 5; i++) {
             result /= divisor;
             suffixIndex++;
         }
         
         /* Format and check for truncation */
         int ret = snprintf(buffer, bufferSize, "%.2f %s", result, suffixes[suffixIndex]);
         if (ret < 0 || ret >= bufferSize) {
             /* Handle truncation safely */
             strncpy(buffer, "ERROR", bufferSize - 1);
             buffer[bufferSize - 1] = '\0';
             return -1;
         }
     }
     
     return 0;
 }
 
 /**
  * Print program usage information
  */
 void print_usage(const char *program_name) {
     printf("Usage: %s [OPTIONS]\n", program_name);
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
     printf("  -h, --human         Show output in human-readable format.\n");
     printf("  -w, --wide          Switch to the wide mode.\n");
     printf("  -c, --count count   Display the result count times. Requires the -s option.\n");
     printf("  -l, --lohi          Show detailed low and high memory statistics.\n");
     printf("  -L, --line          Show output on a single line.\n");
     printf("  -s, --seconds delay Continuously display the result delay seconds apart.\n");
     printf("  --si                Use power of 1000 instead of 1024.\n");
     printf("  -t, --total         Display a line showing the column totals.\n");
     printf("  -v, --committed     Display memory commit information.\n");
     printf("  -d, --debug         Enable debug output.\n");
     printf("  --help              Print help.\n");
     printf("  -V, --version       Display version information.\n");
 }
 
 /**
  * Print program version information
  */
 void print_version(void) {
     printf("%s\n", PROGRAM_VERSION);
 }
 
 /**
  * Main program function
  */
 int main(int argc, char *argv[]) {
     /* Register signal handlers for graceful termination */
     signal(SIGINT, signal_handler);
     signal(SIGTERM, signal_handler);
     
     /* Default parameters */
     int human = 0;
     int si = 0;
     int unit = 1;  /* Default unit is kibibytes (KiB) */
     int wide = 0;
     int count = 1;
     int lohi = 0;
     int line = 0;
     float delay = 1.0;
     int total = 0;
     int committed = 0;
     int debug = 0;
 
     /* Command-line options */
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
         {"debug", no_argument, 0, 'd'},
         {"help", no_argument, 0, '?'},
         {"version", no_argument, 0, 'V'},
         {0, 0, 0, 0}
     };
 
     /* Parse command-line options exactly like Linux free command */
     int opt;
     int option_index = 0;
     opterr = 0; /* Suppress default getopt error messages for Linux-like behavior */
     while ((opt = getopt_long(argc, argv, "bkmgepKMGTPhwc:lLs:StvdV?", long_options, &option_index)) != -1) {
         switch (opt) {
             /* Unit options */
             case 'b': unit = 0; break;
             case 'k': unit = 1; break;
             case 'm': unit = 2; break;
             case 'g': unit = 3; break;
             case 'e': unit = 4; break;
             case 'p': unit = 5; break;
             
             /* SI unit options */
             case 'K': si = 1; unit = 1; break;
             case 'M': si = 1; unit = 2; break;
             case 'G': si = 1; unit = 3; break;
             case 'T': si = 1; unit = 4; break;
             case 'P': si = 1; unit = 5; break;
             
             /* Display options */
             case 'h': human = 1; break;
             case 'w': wide = 1; break;
             case 'l': lohi = 1; break;
             case 'L': line = 1; break;
             case 'S': si = 1; break;
             case 't': total = 1; break;
             case 'v': committed = 1; break;
             case 'd': 
                 debug = 1; 
                 current_log_level = DEBUG;
                 break;
             
             /* Count parameter with validation */
             case 'c': 
                 {
                     char *endptr;
                     long value = strtol(optarg, &endptr, 10);
                     
                     /* Validate conversion and range */
                     if (*endptr != '\0' || value <= 0 || value > MAX_COUNT_VALUE) {
                         log_message(ERROR, "invalid count value\n");
                         CLEANUP_AND_EXIT(EXIT_FAILURE);
                     }
                     count = (int)value;
                 }
                 break;
             
             /* Delay parameter with validation */
             case 's': 
                 {
                     char *endptr;
                     float value = strtof(optarg, &endptr);
                     
                     /* Validate conversion and range */
                     if (*endptr != '\0' || value < MIN_DELAY_VALUE || value > MAX_DELAY_VALUE) {
                         log_message(ERROR, "invalid seconds value\n");
                         CLEANUP_AND_EXIT(EXIT_FAILURE);
                     }
                     delay = value;
                 }
                 break;
             
             /* Help and version options */
             case '?': 
                 print_usage(argv[0]);
                 return EXIT_SUCCESS;
             
             case 'V':
                 print_version();
                 return EXIT_SUCCESS;
             
             default:
                 log_message(ERROR, "Unknown option: %c\n", opt);
                 print_usage(argv[0]);
                 CLEANUP_AND_EXIT(EXIT_FAILURE);
         }
     }
     
     /* Validate -c requires -s */
     if (count > 1 && optind < argc && strcmp(argv[optind-1], "-s") != 0 && 
         !strstr(argv[optind-1], "--seconds")) {
         log_message(ERROR, "option -c/--count requires -s/--seconds\n");
         CLEANUP_AND_EXIT(EXIT_FAILURE);
     }
     
     /* Check for privilege escalation (only show with debug flag) */
     if (getuid() == 0 && debug) {
         log_message(DEBUG, "Running with root privileges\n");
     }
     
     /* Initialize Mach port */
     g_host_port = mach_host_self();
     if (g_host_port == MACH_PORT_NULL) {
         log_message(FATAL, "cannot get host information\n");
         /* FATAL log level automatically exits */
     }
     
     /* Get page size */
     vm_size_t page_size;
     kern_return_t kr = host_page_size(g_host_port, &page_size);
     if (kr != KERN_SUCCESS) {
         log_message(FATAL, "cannot get memory page size\n");
     }
     
     log_message(DEBUG, "Page size: %lu bytes\n", (unsigned long)page_size);
     
     /* Main memory reporting loop */
     for (int i = 0; i < count; i++) {
         /* Fetch total physical memory */
         host_basic_info_data_t hostInfo = {0};
         mach_msg_type_number_t info_count = HOST_BASIC_INFO_COUNT;
         kr = host_info(g_host_port, HOST_BASIC_INFO, (host_info_t)&hostInfo, &info_count);
         if (kr != KERN_SUCCESS) {
             log_message(FATAL, "cannot get memory information\n");
         }
         
         /* Check if detected memory exceeds the practical limit */
         if ((unsigned long long)hostInfo.max_mem > MAX_EXPECTED_MEMORY) {
             log_message(WARNING, "Detected physical memory (%llu bytes) exceeds expected maximum (1TB)\n", 
                        (unsigned long long)hostInfo.max_mem);
         }
         
         log_message(DEBUG, "Total physical memory: %llu bytes\n", 
                    (unsigned long long)hostInfo.max_mem);
         
         /* Fetch detailed memory statistics */
         vm_statistics64_data_t vm_stat = {0};
         mach_msg_type_number_t host_size = sizeof(vm_statistics64_data_t) / sizeof(integer_t);
         kr = host_statistics64(g_host_port, HOST_VM_INFO64, (host_info_t)&vm_stat, &host_size);
         if (kr != KERN_SUCCESS) {
             log_message(FATAL, "cannot get vm statistics\n");
         }
         
         /* Validate memory counts to prevent integer overflow */
         if (vm_stat.free_count < vm_stat.speculative_count) {
             log_message(ERROR, "invalid memory counts\n");
             CLEANUP_AND_EXIT(EXIT_FAILURE);
         }
         
         /* Calculate memory components with overflow checking */
         unsigned long long total_memory = hostInfo.max_mem;
         
         /* Check for potential integer overflow */
         if ((unsigned long long)vm_stat.free_count > ULLONG_MAX / page_size ||
             (unsigned long long)vm_stat.speculative_count > ULLONG_MAX / page_size ||
             (unsigned long long)vm_stat.wire_count > ULLONG_MAX / page_size ||
             (unsigned long long)vm_stat.internal_page_count > ULLONG_MAX / page_size ||
             (unsigned long long)vm_stat.purgeable_count > ULLONG_MAX / page_size ||
             (unsigned long long)vm_stat.external_page_count > ULLONG_MAX / page_size) {
             log_message(FATAL, "integer overflow in memory calculation\n");
         }
         
         unsigned long long free_memory = (unsigned long long)(vm_stat.free_count - vm_stat.speculative_count) * page_size;
         unsigned long long wired_memory = (unsigned long long)vm_stat.wire_count * page_size;
         unsigned long long app_memory = (unsigned long long)(vm_stat.internal_page_count - vm_stat.purgeable_count) * page_size;
         unsigned long long cached_memory = (unsigned long long)(vm_stat.purgeable_count + vm_stat.external_page_count) * page_size;
         
         /* Guard against overflow in subtraction */
         if (total_memory < free_memory + cached_memory) {
             log_message(ERROR, "memory calculation error\n");
             CLEANUP_AND_EXIT(EXIT_FAILURE);
         }
         
         unsigned long long used_memory = total_memory - free_memory - cached_memory;
         
         /* Get swap information safely */
         struct xsw_usage swapinfo = {0};
         size_t swapinfo_sz = sizeof(swapinfo);
         int mib[2] = {CTL_VM, VM_SWAPUSAGE};
         
         if (sysctl(mib, 2, &swapinfo, &swapinfo_sz, NULL, 0) != 0) {
             log_message(ERROR, "cannot get swap information\n");
             /* Continue with zeroed swap info instead of exiting */
             memset(&swapinfo, 0, sizeof(swapinfo));
         }
         
         /* Get memory commit information */
         int mib_commit[2] = {CTL_VM, VM_SWAPUSAGE};
         struct xsw_usage commitinfo = {0};
         size_t commitinfo_sz = sizeof(commitinfo);
         
         if (sysctl(mib_commit, 2, &commitinfo, &commitinfo_sz, NULL, 0) != 0) {
             log_message(ERROR, "cannot get memory commit information\n");
             /* Continue with zeroed commit info instead of exiting */
             memset(&commitinfo, 0, sizeof(commitinfo));
         }
         
         unsigned long long commit_limit = commitinfo.xsu_total;
         unsigned long long committed_memory = commitinfo.xsu_used;
         unsigned long long uncommitted_memory = commitinfo.xsu_avail;
         
         /* Formatting memory sizes with consistent buffer sizes */
         char totalStr[MEMORY_STRING_BUFFER_SIZE], usedStr[MEMORY_STRING_BUFFER_SIZE], 
              freeStr[MEMORY_STRING_BUFFER_SIZE], cachedStr[MEMORY_STRING_BUFFER_SIZE], 
              appStr[MEMORY_STRING_BUFFER_SIZE], wiredStr[MEMORY_STRING_BUFFER_SIZE];
         char swapTotalStr[MEMORY_STRING_BUFFER_SIZE], swapUsedStr[MEMORY_STRING_BUFFER_SIZE], 
              swapFreeStr[MEMORY_STRING_BUFFER_SIZE];
         char commitLimitStr[MEMORY_STRING_BUFFER_SIZE], committedStr[MEMORY_STRING_BUFFER_SIZE], 
              uncommittedStr[MEMORY_STRING_BUFFER_SIZE];
         
         /* Format all memory values with error checking */
         if (formatBytes(total_memory, totalStr, sizeof(totalStr), human, si, unit) < 0 ||
             formatBytes(used_memory, usedStr, sizeof(usedStr), human, si, unit) < 0 ||
             formatBytes(free_memory, freeStr, sizeof(freeStr), human, si, unit) < 0 ||
             formatBytes(cached_memory, cachedStr, sizeof(cachedStr), human, si, unit) < 0 ||
             formatBytes(app_memory, appStr, sizeof(appStr), human, si, unit) < 0 ||
             formatBytes(wired_memory, wiredStr, sizeof(wiredStr), human, si, unit) < 0 ||
             formatBytes(swapinfo.xsu_total, swapTotalStr, sizeof(swapTotalStr), human, si, unit) < 0 ||
             formatBytes(swapinfo.xsu_used, swapUsedStr, sizeof(swapUsedStr), human, si, unit) < 0 ||
             formatBytes(swapinfo.xsu_avail, swapFreeStr, sizeof(swapFreeStr), human, si, unit) < 0 ||
             formatBytes(commit_limit, commitLimitStr, sizeof(commitLimitStr), human, si, unit) < 0 ||
             formatBytes(committed_memory, committedStr, sizeof(committedStr), human, si, unit) < 0 ||
             formatBytes(uncommitted_memory, uncommittedStr, sizeof(uncommittedStr), human, si, unit) < 0) {
             log_message(ERROR, "cannot format memory values\n");
             CLEANUP_AND_EXIT(EXIT_FAILURE);
         }
         
         /* Debug output */
         log_message(DEBUG, "Memory values formatted successfully\n");
         
         /* Printing results in requested format */
         if (line) {
             /* Single line output format, Linux free-like */
             printf("Mem: %s total, %s used, %s free, %s shared, %s buff/cache, %s available\n", 
                    totalStr, usedStr, freeStr, "0B", cachedStr, freeStr);
             printf("Swap: %s total, %s used, %s free\n", 
                    swapTotalStr, swapUsedStr, swapFreeStr);
         } else {
             /* Standard multi-line output format matching Linux free */
             if (wide) {
                 /* Linux free -w format */
                 printf("%-7s %11s %11s %11s %11s %11s %11s %11s\n",
                        "", "total", "used", "free", "shared", "buffers", "cache", "available");
                 /* Use app_memory and wired_memory as buffers and cache for Linux-like output */
                 printf("%-7s %11s %11s %11s %11s %11s %11s %11s\n", 
                        "Mem:", totalStr, usedStr, freeStr, "0B", appStr, cachedStr, freeStr);
                 printf("%-7s %11s %11s %11s\n", 
                        "Swap:", swapTotalStr, swapUsedStr, swapFreeStr);
             } else {
                 /* Standard Linux free format */
                 printf("%-7s %11s %11s %11s %11s %11s %11s\n",
                        "", "total", "used", "free", "shared", "buff/cache", "available");
                 printf("%-7s %11s %11s %11s %11s %11s %11s\n", 
                        "Mem:", totalStr, usedStr, freeStr, "0B", cachedStr, freeStr);
                 printf("%-7s %11s %11s %11s\n", 
                        "Swap:", swapTotalStr, swapUsedStr, swapFreeStr);
             }
             
             /* Optional: low and high memory statistics */
             if (lohi) {
                 if (debug) {
                     log_message(DEBUG, "Low/high memory statistics not implemented\n");
                 }
             }
             
             /* Optional: total statistics (matching Linux free -t format) */
             if (total) {
                 unsigned long long mem_total = total_memory;
                 unsigned long long swap_total = swapinfo.xsu_total;
                 
                 /* Check for potential overflow */
                 if (ULLONG_MAX - mem_total < swap_total) {
                     log_message(ERROR, "integer overflow in total calculation\n");
                 } else {
                     unsigned long long total_total = mem_total + swap_total;
                     
                     /* Format total values consistent with Linux free -t */
                     char totalTotalStr[MEMORY_STRING_BUFFER_SIZE];
                     char totalUsedStr[MEMORY_STRING_BUFFER_SIZE];
                     char totalFreeStr[MEMORY_STRING_BUFFER_SIZE];
                     
                     unsigned long long total_used = used_memory + swapinfo.xsu_used;
                     unsigned long long total_free = free_memory + swapinfo.xsu_avail;
                     
                     if (formatBytes(total_total, totalTotalStr, sizeof(totalTotalStr), human, si, unit) < 0 ||
                         formatBytes(total_used, totalUsedStr, sizeof(totalUsedStr), human, si, unit) < 0 ||
                         formatBytes(total_free, totalFreeStr, sizeof(totalFreeStr), human, si, unit) < 0) {
                         log_message(ERROR, "error formatting total values\n");
                     } else {
                         if (wide) {
                             /* Match the column width of the main output */
                             printf("%-7s %11s %11s %11s %11s %11s %11s %11s\n",
                                    "Total:", totalTotalStr, totalUsedStr, totalFreeStr, 
                                    "", "", "", "");
                         } else {
                             printf("%-7s %11s %11s %11s %11s %11s %11s\n",
                                    "Total:", totalTotalStr, totalUsedStr, totalFreeStr, 
                                    "", "", "");
                         }
                     }
                 }
             }
             
             /* Optional: memory commitment information (matching Linux free -v format) */
             if (committed) {
                 /* Calculate percentage - Linux free shows "% of limit" */
                 double percent = 0;
                 if (commit_limit > 0) {
                     percent = ((double)committed_memory / commit_limit) * 100.0;
                 }
                 
                 /* Match Linux free output format exactly */
                 printf("%-15s %11s\n", "Mem. Limit:", commitLimitStr);
                 printf("%-15s %11s = %.1f%% of limit\n", "Committed:", committedStr, percent);
             }
         }
         
         /* Sleep between iterations if not the last one */
         if (i < count - 1) {
             sleep(delay);
         }
     }
     
     /* Clean up resources */
     cleanup();
     return EXIT_SUCCESS;
 }