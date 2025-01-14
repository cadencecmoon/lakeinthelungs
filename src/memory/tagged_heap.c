#include <lake/ipomoeaalba.h>

AMWAPI s32 iainit(struct ipomoeaalba *ia)
{
    (void)ia;
    return result_success;
}

AMWAPI void iafini(struct ipomoeaalba *ia)
{
    (void)ia;
}

AMWAPI void *iaalloc(struct ipomoeaalba *ia, usize size, usize alignment, u64 tag)
{
    (void)ia;
    (void)size;
    (void)alignment;
    (void)tag;
    return NULL;
}

AMWAPI void *iarealloc(struct ipomoeaalba *ia, void *allocation, usize size, usize alignment, u64 tag)
{
    (void)ia;
    (void)allocation;
    (void)size;
    (void)alignment;
    (void)tag;
    return NULL;
}

AMWAPI void iafree(struct ipomoeaalba *ia, u64 tag)
{
    (void)ia;
    (void)tag;
}
