#pragma once

#include "../bedrock.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Information needed to access and write to the reserved allocation returned by a tagged heap.
 *  This structure can be used for virtually any allocation or suballocation, and it is used 
 *  as the domain of a linear allocator. */
struct allocation {
    usize          offset;  /**< Starts at 0, current offset from the *data address. */
    usize          size;    /**< The combined size of memory in bytes. */
    void          *data;    /**< The beginning of the memory. */
    rivens_chain_t chain;   /**< A synchronization primitive for use with Riven. */
};

/** Implements some simple allocations on a common allocation structure. */
LAKEAPI void *allocate_aligned_synchronized(
    struct allocation *alloc, 
    usize              size, 
    usize              alignment, 
    struct riven      *riven)
    attr_hot attr_nonnull(1);

/** Linearly suballocate memory of a minimum size without alignment rules, internally synchronized. */
INLINE attr_nonnull(1) 
void *allocate_synchronized(struct allocation *alloc, usize size, struct riven *riven) 
{
    return allocate_aligned_synchronized(alloc, size, 1, riven);
}

/** Linearly suballocate memory of a minimum size, with address aligned to an alignment rule. */
INLINE attr_nonnull(1)
void *allocate_aligned(struct allocation *alloc, usize size, usize alignment) 
{
    return allocate_aligned_synchronized(alloc, size, alignment, NULL);
}

/** Linearly suballocate memory of a minimum size without alignment rules. */
INLINE attr_nonnull(1)
void *allocate(struct allocation *alloc, usize size) 
{
    return allocate_aligned_synchronized(alloc, size, 1, NULL);
}

/** Copy a NULL-terminated string into a given allocation. */
LAKEAPI void *allocate_cstr(
    struct allocation *alloc, 
    const char        *cstr,
    struct riven      *riven) 
    attr_hot attr_nonnull(1,2);

#ifdef __cplusplus
}
#endif
