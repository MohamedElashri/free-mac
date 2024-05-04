#include <stdio.h>
#include <string.h>
#include "format_utils.h"
#include "memory_info.h"

void test_format_bytes() {
    char buffer[50];
    int buffer_size = sizeof(buffer);

    // Test case 1: Default output (kibibytes)
    format_bytes(1024, buffer, buffer_size, 0, 0, 1);
    printf("Test case 1: ");
    if (strstr(buffer, "1.00 KiB") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1.00 KiB\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }

    // Test case 2: Bytes (-b, --bytes)
    format_bytes(1024, buffer, buffer_size, 0, 0, 0);
    printf("Test case 2: ");
    if (strstr(buffer, "1024.00 B") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1024.00 B\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }

    // Test case 3: Mebibytes (-m, --mebi)
    format_bytes(1024 * 1024, buffer, buffer_size, 0, 0, 2);
    printf("Test case 3: ");
    if (strstr(buffer, "1.00 MiB") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1.00 MiB\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }

    // Test case 4: Gibibytes (-g, --gibi)
    format_bytes(1024 * 1024 * 1024, buffer, buffer_size, 0, 0, 3);
    printf("Test case 4: ");
    if (strstr(buffer, "1.00 GiB") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1.00 GiB\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }

    // Test case 5: Human-readable format (-h, --human)
    format_bytes(1024 * 1024 * 1024, buffer, buffer_size, 1, 0, 0);
    printf("Test case 5: ");
    if (strstr(buffer, "1GiB") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1GiB\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }

    // Test case 6: SI units (--si)
    format_bytes(1024 * 1024, buffer, buffer_size, 0, 1, 2);
    printf("Test case 6: ");
    if (strstr(buffer, "1.05 MB") != NULL) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
        printf("  Expected output: \"1.05 MB\"\n");
        printf("  Actual output: \"%s\"\n", buffer);
    }
}

void test_get_memory_info() {
    MemoryInfo info = get_memory_info();

    // Test case 7: Check if total memory is greater than 0
    printf("Test case 6: ");
    if (info.total_memory > 0) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }

    // Test case 8: Check if free memory is less than or equal to total memory
    printf("Test case 7: ");
    if (info.free_memory <= info.total_memory) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }

    // Test case 9: Check if used memory plus free memory plus cached memory equals total memory
    printf("Test case 8: ");
    unsigned long long sum = info.used_memory + info.free_memory + info.cached_memory;
    if (sum == info.total_memory) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }

    // Test case 10: Check if swap total is greater than or equal to swap used
    printf("Test case 9: ");
    if (info.swapinfo.xsu_total >= info.swapinfo.xsu_used) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }

    // Test case 11: Check if committed memory is less than or equal to commit limit
    printf("Test case 10: ");
    if (info.committed_memory <= info.commit_limit) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }
}

int main() {
    printf("Running unit tests...\n");
    
    printf("Testing format_utils:\n");
    test_format_bytes();
    
    printf("\nTesting memory_info:\n");
    test_get_memory_info();

    return 0;
}