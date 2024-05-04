#include "memory_info.h"
#include <stdio.h>
#include <stdlib.h>


MemoryInfo get_memory_info() {
    mach_port_t host_port = mach_host_self();
    vm_size_t page_size;
    kern_return_t kr;

    kr = host_page_size(host_port, &page_size);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get page size, code %d\n", kr);
        exit(1);
    }
        
    // Fetch total physical memory
    host_basic_info_data_t hostInfo = {0};
    mach_msg_type_number_t info_count = HOST_BASIC_INFO_COUNT;
    kr = host_info(host_port, HOST_BASIC_INFO, (host_info_t)&hostInfo, &info_count);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get total memory, code %d\n", kr);
        exit(1);
    }

    // Fetch detailed memory statistics
    vm_statistics64_data_t vm_stat = {0};
    mach_msg_type_number_t host_size = sizeof(vm_statistics64_data_t) / sizeof(integer_t);
    kr = host_statistics64(host_port, HOST_VM_INFO64, (host_info_t)&vm_stat, &host_size);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Error: Failed to get memory statistics, code %d\n", kr);
        exit(1);
    }

    // Calculate memory components
    unsigned long long total_memory = hostInfo.max_mem;
    unsigned long long free_memory = (unsigned long long)(vm_stat.free_count - vm_stat.speculative_count) * page_size;
    unsigned long long wired_memory = (unsigned long long)vm_stat.wire_count * page_size;
    unsigned long long app_memory = (unsigned long long)(vm_stat.internal_page_count - vm_stat.purgeable_count) * page_size;
    unsigned long long cached_memory = (unsigned long long)(vm_stat.purgeable_count + vm_stat.external_page_count) * page_size;
    unsigned long long used_memory = total_memory - free_memory - cached_memory;
    unsigned long long shared_memory = 0;
    unsigned long long available_memory = free_memory + cached_memory;
    // Get swap information
    struct xsw_usage swapinfo;
    size_t swapinfo_sz = sizeof(swapinfo);
    int mib[2] = {CTL_VM, VM_SWAPUSAGE};
    if (sysctl(mib, 2, &swapinfo, &swapinfo_sz, NULL, 0) != 0) {
        perror("Error: sysctl failed to get swap usage");
        exit(1);
    }

    // Get memory commit limit and amount of committed memory
    int mib_commit[2] = {CTL_VM, VM_SWAPUSAGE};
    struct xsw_usage commitinfo;
    size_t commitinfo_sz = sizeof(commitinfo);
    if (sysctl(mib_commit, 2, &commitinfo, &commitinfo_sz, NULL, 0) != 0) {
        perror("Error: sysctl failed to get memory commit info");
        exit(1);
    }

    unsigned long long commit_limit = commitinfo.xsu_total;
    unsigned long long committed_memory = commitinfo.xsu_used;
    unsigned long long uncommitted_memory = commitinfo.xsu_avail;

    MemoryInfo info = {
        .total_memory = total_memory,
        .free_memory = free_memory,
        .wired_memory = wired_memory,
        .app_memory = app_memory,
        .cached_memory = cached_memory,
        .used_memory = used_memory,
        .shared_memory = shared_memory,
        .available_memory = available_memory,
        .swapinfo = swapinfo,
        .commit_limit = commit_limit,
        .committed_memory = committed_memory,
        .uncommitted_memory = uncommitted_memory
    };

    return info;
    return info;
}