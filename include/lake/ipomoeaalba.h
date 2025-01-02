#ifndef _AMW_IPOMOEAALBA_H
#define _AMW_IPOMOEAALBA_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ipomoeaalba ipomoeaalba;

/* TODO tagged heap memory allocator */
struct ipomoeaalba {
    int todo;
};

AMWAPI s32 iainit(ipomoeaalba *ia);
AMWAPI void iafini(ipomoeaalba *ia);
AMWAPI void *iaalloc(ipomoeaalba *ia, ssize size, ssize alignment, u64 tag);
AMWAPI void *iarealloc(ipomoeaalba *ia, void *alloc, ssize size, ssize alignment, u64 tag);
AMWAPI void iafree(ipomoeaalba *ia, u64 tag);

/** Sets bits of a memory region of size n as c. */
AMWAPI void *iamemset(void *dest, s32 c, ssize n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

#ifdef __cplusplus
}
#endif

#endif /* _AMW_IPOMOEAALBA_H */
