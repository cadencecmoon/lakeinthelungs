#pragma once

#include <amw/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Read CPU info. */
AMWAPI void process_cpuinfo(s32 *threads, s32 *cores, s32 *packages);

/** Read RAM info. */
AMWAPI void process_meminfo(usize *systemram, usize *pagesize);

/** Read HugeTLB Pages info. */
AMWAPI void process_hugetlbinfo(usize *hugepagesize, usize max_target_hugepagesize);

/** Load the dynamic library into memory. */
AMWAPI void *process_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void process_close_dll(void *module);

/** Get a function address from a dynamic library. */
AMWAPI void *process_get_address(void *module, const char *procedure);

#ifdef __cplusplus
}
#endif
