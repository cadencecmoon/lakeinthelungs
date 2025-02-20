#pragma once

#include <amw/bedrock.h>

/** Read CPU info. */
AMWAPI void proc_cpuinfo(s32 *threads, s32 *cores, s32 *packages);

/** Read RAM info. */
AMWAPI void proc_meminfo(usize *systemram, usize *pagesize);

/** Read HugeTLB Pages info. */
AMWAPI void proc_hugetlbinfo(usize *hugepagesize, usize max_target_hugepagesize);
