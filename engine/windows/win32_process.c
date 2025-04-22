#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>

void bedrock_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages)
{
    /* TODO hardcoded my laptops values */
    s32 threads = 12;
    s32 cores = 6;
    s32 packages = 1;

    if (out_threads)  *out_threads = threads;
    if (out_cores)    *out_cores = cores;
    if (out_packages) *out_packages = packages;
}

void bedrock_meminfo(usize *out_total_ram, usize *out_pagesize)
{
    ssize page, bytes;

    /* TODO hardcoded my laptops values */
    page = 4096lu;
    bytes = 16lu*1024*1024*1024;

    if (out_total_ram) *out_total_ram = bytes;
    if (out_pagesize)  *out_pagesize = page;
}

void bedrock_hugetlbinfo(usize *out_huge_pagesize, usize max_accept_size)
{
    if (!out_huge_pagesize) return;

    /* TODO hardcoded my laptops values */
    if (hugepagesize && max_target_hugepagesize >= 2*1024*1024)
        *hugepagesize = 2*1024*1024;
    else if (hugepagesize)
        *hugepagesize = 0;
}
#endif /* LAKE_PLATFORM_WINDOWS */
