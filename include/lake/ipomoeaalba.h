/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_IPOMOEAALBA_H
#define _AMW_IPOMOEAALBA_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO tagged heap memory allocator. */
struct ipomoeaalba {
    int todo;
};

AMWAPI s32 iainit(struct ipomoeaalba *ia);
AMWAPI void iafini(struct ipomoeaalba *ia);
AMWAPI void *iaalloc(struct ipomoeaalba *ia, usize size, usize alignment, u64 tag);
AMWAPI void *iarealloc(struct ipomoeaalba *ia, void *alloc, usize size, usize alignment, u64 tag);
AMWAPI void iafree(struct ipomoeaalba *ia, u64 tag);

/** Copies a memory region of size bytes from dest to src. */
AMWAPI void *iamemcpy(void *restrict dest, const void *restrict src, usize n);

/** Sets bits of a memory region of size bytes as c. */
AMWAPI void *iamemset(void *restrict dest, s32 c, usize n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

#ifdef __cplusplus
}
#endif

#endif /* _AMW_IPOMOEAALBA_H */
