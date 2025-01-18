/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_IPOMOEAALBA_H
#define _AMW_IPOMOEAALBA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Different expected lifetimes, can be used either directly with iaalloc() or 
 *  a defined tag could be incremented within the given range. */
enum ipomoea_tag {
    ipomoea_tag_free            = 0llu,             /**< The block is free to be owned by an unique tag. */
    ipomoea_tag_root            = UINT64_MAX,       /**< Used for allocations that are expected to share the lifetime of the application. */
    ipomoea_tag_scratch         = 0x009F0000llu,    /**< Used for internal scratch blocks, a block is owned by a thread index. */
    ipomoea_tag_workload        = 0x00A10000llu,    /**< Lifetime of an entire workload in the gameloop, for data used between stages. */
    ipomoea_tag_simulation      = 0x00A20000llu,    /**< Lifetime of a single workload's simulation stage. */
    ipomoea_tag_rendering       = 0x00A40000llu,    /**< Lifetime of a single workload's rendering stage. */
    ipomoea_tag_gpuexec         = 0x00A80000llu,    /**< Lifetime of a single workload's GPU execution stage. */
};

/** The header of a tagged heap memory page. It points to the memory block it owns, and
 *  pairs it with a tag - the tag can be anything, an unique tag describes the lifetime 
 *  of resources allocated within a block, and only allocations sharing this tag will be 
 *  suballocated within a block owned by the same tag. */
struct tagged_heap_header {
    at_u64                      tag;            /**< An unique tag describes the lifetime of memory allocations. A tag of 0 means the block is free. */
    at_usize                    used;           /**< Total bytes suballocated within the memory block. */
    void                       *page;           /**< The memory block owned by this tag. */
    /** The next header, and the next block in the continuous memory of the tagged heap.
     *  We resolve all blocks in a tagged heap at creation time, to make sure all the 
     *  pages are continuous in memory. They are used within a context of the heap, 
     *  so we don't need any atomic synchronization here. */
    struct tagged_heap_header  *next;
};

/** A heap manages continuous memory blocks */
struct tagged_heap {
    struct tagged_heap_header      *pages;          /**< Points to the beginning of the allocated memory. */
    u32                             page_count;     /**< How many continuous pages are available from this heap. */
    AMW_ATOMIC(struct tagged_heap *)next;           /**< Next heaps can be created if all blocks become occupied. */
};

/** Ipomoea Alba is a block allocator that greatly simplifies data lifetime and memory 
 *  management of transient data in a multithreaded environment. This structure is responsible 
 *  for managing different strategies of memory allocation.
 *
 *  TODO find an efficient way to access the tagged blocks:
 *  - Does a block with such a tag exist already?
 *  - If yes, is this block full or can something be allocated? 
 *  - How to tell how many blocks are owned by a specific tag?
 *  - How to avoid 'zombie' memory blocks, whose tags were never freed for some reason?
 *  - Can i manage the memory allocations better somehow, to waste less memory? */
struct ipomoea {
    struct tagged_heap              root_heap;              /**< Memory pool for data that shares the lifetime of this allocator. */
    struct tagged_heap              scratch_heap;           /**< A heap for thread-local temporary data. */
    AMW_ATOMIC(struct tagged_heap *)heaps;                  /**< The head of the dynamic tagged heap allocators. */
    thread_t                       *threads;                /**< The array of worker threads that may access this allocator. */
    ssize                           thread_count;           /**< Used to retrieve the thread index, corresponds to the total pages in this heap. */
    usize                           page_size;              /**< The size of a memory block in this heap, usually 2 MiB (large page size). */
    usize                           max_memory_budget;      /**< Maximum memory in bytes that this allocator can map. */
    at_usize                        ram_budget;             /**< The estimated budget of RAM memory in bytes that is left in the system. */
};

/** Initialize the first memory heap, used for the lifetime of this allocator. This heap 
 *  has blocks owned by the ipomoea_tag_root. The root memory budget is an estimate of
 *  how much memory will be needed for the internal subsystems of this application.
 *  The page size will be used as the size of the memory blocks in the heaps. The memory 
 *  limit is used to physically limit how much RAM can be mapped for use by this allocator. */ 
AMWAPI struct ipomoea *iainit(usize max_memory_budget, usize root_memory_estimate);

/** Frees all system memory used within ipomoea. Use this only after all dynamic memory 
 *  that have been suballocated has finished its work. */
AMWAPI void iafini(struct ipomoea *ia);

/** Assemble a scratch allocator using one block per worker thread. */
AMWAPI s32 iassemble_scratch(struct ipomoea *ia, thread_t *threads, ssize thread_count);

/** Suballocate memory under a block with this tag. If no block with such tag is available,
 *  or if the allocation is over 2MiB (large page size), the first free block will be picked.
 *  Requesting an alignment of 2MiB or more will result in taking a free block immediately.
 *  If an allocation greatly exceeds the memory budget of existing heaps, it will become a 
 *  dedicated allocation with an innate 2MiB alignment. */
AMWAPI void *ialloc(struct ipomoea *ia, usize size, usize alignment, u64 tag);

/** Suballocate memory on a scratch block owned by the thread index. The maximum
 *  lifetime of such memory is 1 frame within the domain of a gameloop stage.
 *  Don't make large (especially over 2MiB) allocations in the scratch blocks. */
AMWAPI void *iascratch(struct ipomoea *ia, usize size, usize alignment);

/** Frees all blocks owned by a given memory tag and zeroes out memory. The root tag is ignored. */
AMWAPI void iafree(struct ipomoea *ia, u64 tag);

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
