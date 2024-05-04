#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <mach/mach.h>
#include <sys/types.h>
#include <sys/sysctl.h>

typedef struct {
    unsigned long long total_memory;
    unsigned long long free_memory;
    unsigned long long wired_memory;
    unsigned long long app_memory;
    unsigned long long cached_memory;
    unsigned long long used_memory;
    struct xsw_usage swapinfo;
    unsigned long long commit_limit;
    unsigned long long committed_memory;
    unsigned long long uncommitted_memory;
} MemoryInfo;

MemoryInfo get_memory_info();

#endif