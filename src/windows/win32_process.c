#ifdef PLATFORM_WINDOWS
#include <amw/bedrock/log.h>
#include <amw/bedrock/process.h>

#include "win32_hadal.h"

void *process_load_dll(const char *libname)
{
    if (!libname)
        return NULL;

    HMODULE *module = LoadLibraryA(libname);
    if (!module)
        log_error("LoadLibraryA '%s' failed (error %lu).", libname, GetLastError());
    return (void *)module;
}

void process_close_dll(void *module)
{
    if (module) FreeLibrary((HMODULE)module);
}

void *process_get_address(void *module, const char *procedure)
{
    if (!module || !procedure)
        return NULL;
    return GetProcAddress((HMODULE)module, procedure);
}

void process_cpuinfo(s32 *threads, s32 *cores, s32 *packages)
{
    /* TODO hardcoded my laptops values */
    if (threads)
        *threads = 12;
    if (cores)
        *cores = 6;
    if (packages)
        *packages = 1;
}

void process_meminfo(usize *systemram, usize *pagesize)
{
    /* TODO hardcoded my laptops values */
    if (systemram)
        *systemram = 16lu*1024*1024*1024;
    if (pagesize)
        *pagesize = 4096;
}

void process_hugetlbinfo(usize *hugepagesize, usize max_target_hugepagesize)
{
    /* TODO hardcoded my laptops values */
    if (hugepagesize && max_target_hugepagesize >= 2*1024*1024)
        *hugepagesize = 2*1024*1024;
    else if (hugepagesize)
        *hugepagesize = 0;
}
#endif /* PLATFORM_WINDOWS */
