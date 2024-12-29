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

#ifndef AMW_NO_PROTOTYPES

AMWAPI int32_t AMWAPIENTRY iainit(struct ipomoeaalba *ia);
AMWAPI void AMWAPIENTRY iafini(struct ipomoeaalba *ia);

AMWAPI void * AMWAPIENTRY iaalloc(struct ipomoeaalba *ia, size_t size, size_t alignment, uint64_t tag);
AMWAPI void * AMWAPIENTRY iarealloc(struct ipomoeaalba *ia, void *allocation, size_t size, size_t alignment, uint64_t tag);
AMWAPI void AMWAPIENTRY iafree(struct ipomoeaalba *ia, uint64_t tag);

/** Sets bits of a memory region of size n as c. */
AMWAPI void * AMWAPIENTRY iamemset(void *dest, int32_t c, size_t n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

#endif /* AMW_NO_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_IPOMOEAALBA_H */
