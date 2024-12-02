/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_MEMORY_H
#define _AMW_MEMORY_H

#include <amw/common/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

AMWAPI void * AMWCALL iamalloc__(size_t size);
AMWAPI void * AMWCALL iacalloc__(size_t size, size_t n);
AMWAPI void * AMWCALL iarealloc__(void *ptr, size_t new_size);
AMWAPI void   AMWCALL iafree__(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_MEMORY_H */
