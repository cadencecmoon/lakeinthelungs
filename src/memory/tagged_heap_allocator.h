#pragma once

#include "../bedrock.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** We use a fixed 2MB size per block. */
#define TAGGED_HEAP_BLOCK_SIZE (2 * 1024 * 1024)

/** An unique tag groups blocks with a shared resource lifetime. Different tags */
enum heap_tag {
    /** This block is either free or represents an uncommited physical page. */
    heap_tag_free__     = 0x0000u,
    /** Used for allocations that are expected to share the lifetime of the tagged heap.
     *  Freeing the root tag may effectively bomb all resources, XXX we'll make it a NO-OP for now. */
    heap_tag_roots__    = 0x0001u,   
    /** Cleanup pools of the tear allocator, may be used at synchronization points (between stages). */
    heap_tag_tears__    = 0x0002u,

    heap_tag_lake_work  = 0x0100u,  /**< Reset at the beginning of this frame, spans across all stages. */
    heap_tag_simulation = 0x0200u,  /**< Reset at the end of a simulation stage for this frame. */
    heap_tag_rendering  = 0x0300u,  /**< Reset at the end of a rendering stage for this frame. */
    heap_tag_gpuexec    = 0x0400u,  /**< Reset at the end of a GPU execution stage for this frame. */
};

/** The allocator gets rid of synchronization overhead, by keeping seperate memory pools
 *  available from a thread index. No external synchronization is needed either. The tear 
 *  allocator is viable only for temporary and small allocations. If the memory pool 
 *  capacity a thread index is exceeded, the offset will flush back to 0 - or if the 
 *  allocation attempts to exceed the page size, a fatal assertion is issued. Tear pages 
 *  should hold enough memory that is expected to be allocated during the frequency of 
 *  resets done to the tear allocator. A free applies to all pages in the allocator. */
struct tears_allocator {
    void                       *data;           /**< Beginning of the heap_tag_tear__ blocks */
    usize                      *offsets;        /**< An array of offsets for suballocation. */
    thread_t                   *threads;        /**< An array of threads given from riven. */
    ssize                       thread_count;   /**< How many threads are in the system. */
};

/** Pairs an allocation structure with a linked list. */
struct roots_page {
    struct roots_page          *next;           /**< The next region of memory with heap_tag_root__. */
    struct allocation           alloc;          /**< A region of memory blocks. */
};

/** For now we'll be using a simple linear allocator for our root allocations. */
struct roots_allocator {
    struct roots_page           head;           /**< The first page. */
    struct roots_page          *tail;           /**< The last page, from where we are allocating. */
};

/** Flags to control the state of the allocator. */
enum tagged_heap_flags {
    /** The allocator was initialized and is ready to work. */
    tagged_heap_flag_is_valid           = (1u << 0),
    /** Run in the context of a Riven job system - work will be parallelized, all write-modify 
     *  access points synchronized by context switches, and the tear allocator will be accessible. */
    tagged_heap_flag_parallel_workflow  = (1u << 1),
    /** Instead of asserting an error, tear pages will wrap on overflows instead, with a warning message.
     *  This flag is ignored for NDEBUG builds. */
    tagged_heap_wrap_tear_pages         = (1u << 2),
};

/** It's all about managing different lifetimes. A simple block allocator that will serve as 
 *  a common backend for other memory allocators. */
struct tagged_heap_allocator {
    at_u16                     *tags;           /**< The array of heap tags, divided within batches. One tag per block. */
    at_u8                      *free_bitmap;    /**< The array of free block bitmaps, divided within batches. One byte represents 8 blocks. */
    rivens_chain_t             *chains;         /**< Synchronization primitives for write-modify access. One chain per 128MB. */
    void                       *mapping;        /**< Beginning of the upfront mapped virtual memory. */

    usize                       map_budget;     /**< How much memory is this allocator allowed to commit. This virtual address space is mapped upfront. */
    usize                       page_size;      /**< The page size used for virtual memory mapping, by default 2MB is used. */
    rivens_chain_t              growth_chain;   /**< This lock is used for managing kernel calls (mmap, VirtualAlloc, etc.). */
    at_u32                      flags;          /**< Flags controling the state and patterns used by this allocator. */

    struct roots_allocator      roots;          /**< To allocate any size of resources from the heap tag root memory blocks. */
    struct tears_allocator      tears;          /**< Thread-local memory pools for fast, small and temporary allocations. */
    struct riven               *riven;          /**< A lot of work can be parallelized, and we use chains for synchronization. */
};

/** Configure the behaviour of this allocator. This may include patterns of mapping new memory,
 *  preparing structures for batches and internal allocators, preallocating arrays of tags and 
 *  free bitmaps, with respect to the given RAM budget for this application. This function is 
 *  expected to be called before Riven is created and has entered the main procedure.
 *  Don't call this function more than once, before calling thfini(). */
LAKEAPI s32 thinit( // TODO
    struct tagged_heap_allocator *th,
    usize                         expected_root_usage)
    attr_nonnull(1);

/** Configure the tagged heap allocators for use within a parallel workflow. Don't call this 
 *  function more than once, before calling thfini(), and before calling thinit(). */
LAKEAPI void thinitlate(
    struct tagged_heap_allocator *th,
    struct riven                 *riven,
    thread_t                     *threads,
    ssize                         thread_count)
    attr_nonnull_all;

/** TODO Try to commit physical pages for new resources. A new heap batch may be created. */
LAKEAPI s32 thgrow(struct tagged_heap_allocator *th) attr_nonnull(1);

/** TODO Try to uncommit physical pages to free system resources. */
LAKEAPI s32 thtrim(struct tagged_heap_allocator *th) attr_nonnull(1);

/** Unmaps all memory and clears the allocator structure. Calling this is enough to 
 *  cleanup resources used in the lifetime of this allocator (and possibly application). */
LAKEAPI void thfini(struct tagged_heap_allocator *th) attr_nonnull(1);

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
    u16                           tag, 
    u32                           reserve_2mb_count, 
    struct allocation            *dst) 
    attr_hot attr_nonnull(1,4);

/** Use the linear 'root' allocator to suballocate memory in the internally reserved memory block.
 *  This memory is expected to hold resources that share the lifetime of this tagged heap. */
LAKEAPI void *throots(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
    attr_hot attr_nonnull(1);

/** Temporarily allocate from a thread-local memory pool, for a size and alignment requirement. 
 *  No external nor internal synchronization is necessary. Alignment must be a power of 2. */
LAKEAPI void *thtears(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
    attr_hot attr_nonnull(1);

/** Reclaim all blocks of memory that were reserved with the given tag. This memory can later be 
 *  reused by other resources. Using previously suballocated resources after freeing the tag used 
 *  by them is undefined behaviour and NOT save. Some tags have unique behaviour */
LAKEAPI void thfree(
    struct tagged_heap_allocator *th, 
    u16                           tag) 
    attr_hot attr_nonnull(1);

#ifdef __cplusplus
}
#endif
