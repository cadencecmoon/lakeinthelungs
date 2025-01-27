#pragma once

#include "../bedrock.h"

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** We use a fixed 2MB size per block. */
#define TAGGED_HEAP_BLOCK_SIZE (2 * 1024 * 1024)

/** Limit a batch to 512 blocks of 2MB size. */
#define TAGGED_HEAP_MAX_BATCH_SIZE (1024 * 1024 * 1024)

/** An unique tag groups blocks with a shared resource lifetime. */
enum heap_tag {
    heap_tag_free       = 0u,           /**< This heap page is free to claim new ownership. */
    heap_tag_lake_work  = 0x00A10000u,  /**< Reset at the beginning of this frame, spans across all stages. */
    heap_tag_simulation = 0x00A20000u,  /**< Reset at the end of a simulation stage for this frame. */
    heap_tag_rendering  = 0x00A30000u,  /**< Reset at the end of a rendering stage for this frame. */
    heap_tag_gpuexec    = 0x00A40000u,  /**< Reset at the end of a GPU execution stage for this frame. */
    heap_tag_root       = UINT32_MAX,   /**< Used for allocations that are expected to share the lifetime of the tagged heap. */
};

/** A batch combines multiple 2MB of memory blocks into a single mapping. Instead of a free-list,
 *  to check for free blocks a bitmap is used, with each bit corresponding to a single 2MB block.
 *  Batches are managed independently, this allows the tagged heap to grow or trim during the life 
 *  of an application, for the allocations to be contiguous in memory even if larger than 2MB, and 
 *  it allows us to group the metadata (tags, free-bitmaps) into small arrays. Batches of different 
 *  sizes are supported, but we assume it is no larger than 512mb. A minimum of 128mb batch is a 
 *  comfortable size, but smaller batches can work too. */
struct tagged_heap_batch {
    struct tagged_heap_batch *next;             /**< Next batch in a linked list. */
    rivens_chain_t            chain;            /**< This chain will block with a fiber context switch. */
    at_u32                   *tags;             /**< An array of tags, one tag per block. */
    at_u64                    free_bitmap[8];   /**< A bit of 1 means free, 0 means block is reserved/in use. One 64-bit value  represents 64 blocks. */
    usize                     block_count;      /**< A block is in a fixed 2MB of size, this many blocks are available in this batch. */
    usize                     page_size;        /**< The page size used for this mapping. 2MB is by default, but 4096KB, 1MB, 4MB or 1GB may be used too. */
    void                     *memory;           /**< The beginning of the mapped allocation this batch describes. */
};

/** It's all about managing different lifetimes. A simple block allocator that will serve as 
 *  a common backend for other memory allocators. */
struct tagged_heap_allocator {
    struct tagged_heap_batch *batches;          /**< Linked list of batch allocations. */
    struct allocation         root;             /**< Points to the last root block, reallocated internally if it becomes full. */

    rivens_chain_t            chain;            /**< This lock is used for managing kernel calls (mmap, VirtualAlloc, etc.). */
    struct riven             *riven;            /**< Lots of work can be either parallelized, or synchronized with context switches instead of locks. */
};

/** Reserves count of 2MB memory blocks and returns a contiguous allocation in this range.
 *  The given heap_tag will be used for this allocation, and resources can be freed with a 
 *  thfree(heap_tag) call. This function can only give free blocks in a multiple of 2MB size.
 *  This function will NOT try to map new memory pages, if an error occurs.
 *  
 *  @return Non-zero value if the allocation has failed for some reason.
 *
 *  'result_out_of_memory' - On all available tagged heap batches, no blocks were free to reserve;
 *  'result_fragmented'    - Could not find enough contiguous blocks of memory to satisfy the allocation; */
LAKEAPI s32 thmalloc(
    struct tagged_heap_allocator *th, 
    u32                           heap_tag, 
    u32                           reserve_2mb_count, 
    struct allocation            *dst) 
    attr_hot attr_nonnull(1,4);

/** Use the linear 'root' allocator to suballocate memory in the internally reserved memory block.
 *  This memory is expected to hold resources that share the lifetime of this tagged heap, or even 
 *  the application, and cannot be freed with thfree(heap_tag_root). */
LAKEAPI void *throots(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
    attr_hot attr_nonnull(1);

/** Reclaim all blocks of memory that were reserved with the given tag. This memory can later be 
 *  reused by other resources. Using previously suballocated resources after freeing the tag used 
 *  by them is undefined behaviour and NOT save. */
LAKEAPI void thfree(
    struct tagged_heap_allocator *th, 
    u32                           tag) 
    attr_hot attr_nonnull(1);

#ifdef __cplusplus
}
#endif
