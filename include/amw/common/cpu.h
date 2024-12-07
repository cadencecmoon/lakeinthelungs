/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_CPU_H
#define _AMW_CPU_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

AMWAPI void cpu_count(size_t *threads, size_t *cores, size_t *packages);

AMW_C_DECL_END
#endif /* _AMW_CPU_H */
