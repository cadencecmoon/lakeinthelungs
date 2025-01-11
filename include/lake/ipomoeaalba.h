/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_IPOMOEAALBA_H
#define _AMW_IPOMOEAALBA_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO tagged heap memory allocator */
struct ipomoeaalba {
    int todo;
};

AMWAPI s32 iainit(struct ipomoeaalba *ia);
AMWAPI void iafini(struct ipomoeaalba *ia);
AMWAPI void *iaalloc(struct ipomoeaalba *ia, ssize size, ssize alignment, u64 tag);
AMWAPI void *iarealloc(struct ipomoeaalba *ia, void *alloc, ssize size, ssize alignment, u64 tag);
AMWAPI void iafree(struct ipomoeaalba *ia, u64 tag);

/** Sets bits of a memory region of size n as c. */
AMWAPI void *iamemset(void *dest, s32 c, ssize n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

#ifdef __cplusplus
}
#endif

#endif /* _AMW_IPOMOEAALBA_H */
