#pragma once

#include "../bedrock.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A page of memory used in the tear allocator. */
struct tear_page {
    usize offset;   /**< Starts at 0, current offset from the *data address. */
    usize size;     /**< The combined size of memory in bytes. */
    void *data;     /**< The beginning of the memory. */
};

/** The allocator gets rid of synchronization overhead, by keeping seperate memory pools
 *  available from a thread index. No external synchronization is needed either. The tear 
 *  allocator is viable only for temporary and small allocations. If the memory pool 
 *  capacity a thread index is exceeded, the offset will flush back to 0 - or if the 
 *  allocation attempts to exceed the page size, a fatal assertion is issued. Tear pages 
 *  should hold enough memory that is expected to be allocated during the frequency of 
 *  resets done to the tear allocator. A free applies to all pages in the allocator. */
struct tear_allocator {
    struct tear_page   *pages;          /**< An array of tear pages, one page per thread. */
    thread_t           *threads;        /**< An array of threads given from riven. */
    ssize               thread_count;   /**< How many threads are in the system. */
};

/** Temporarily allocate from a thread-local memory pool, for a size and alignment requirement. 
 *  No external nor internal synchronization is necessary. Alignment must be a power of 2. */
LAKEAPI void *tear_alloc_aligned(
    struct tear_allocator *tear, 
    usize                  size, 
    usize                  alignment) 
    attr_hot attr_nonnull(1);

/** Temporarily allocate from a thread-local memory pool, without alignment rules. */
INLINE attr_nonnull(1)
void *tear_alloc(struct tear_allocator *tear, usize size) 
{
    return tear_alloc_aligned(tear, size, 1);
}

/** Sets offsets of all tear pages in the allocator to zero, freeing resources
 *  for new allocations. This function should be called within synchronization points,
 *  between iterations of the gameloop for example. */
LAKEAPI void tear_free(struct tear_allocator *tear) attr_nonnull(1);

#ifdef __cplusplus
}
#endif
