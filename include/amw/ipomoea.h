/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_IPOMOEA_H
#define _AMW_IPOMOEA_H

#include <amw/common/defines.h>
#include <amw/riven.h>

AMW_C_DECL_BEGIN

/** Sets bits of a memory region of size n as c. */
AMWAPI void *iamemset(void *dest, i32 c, size_t n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

/** Types of memory allocations in AMW, indicating shared memory lifetime.
 *  Some tags are unique and have their restrictions listed below. 
 *
 *  Custom allocators all derive from a chunk of tagged memory, and manage 
 *  this chunk in their own way (arena, stack, list, etc.). Ipomoeaalba 
 *  will make sure to finish such custom allocators when their internal 
 *  memory lifetime ends, together with finishing a memory tag.
 *
 *  Tags can have a range of values (e.g. memory_tag + index) where each 
 *  shares the same purpose, but for different lifetimes managed by the 
 *  engine. An example of such tag is memory_tag_frame_simulation, that 
 *  can be paired with an index of the framedata array.
 *
 *  A memory tag used in the tagged heap allocator can be anything, 
 *  the enumerations below should serve as a common interface.
 *
 *  Passing in a common tag like memory_tag_frame_simulation will increment the 
 *  tag by the current thread index, but only if more than 1 worker thread 
 *  is running. This ensures every thread owns a private memory block when 
 *  data is expected to be not synchronized. */
enum memory_tag {
    memory_tag_free             = 0x00000000llu,            /**< This page is free to be claimed by another tag. */

    /* Values come from the assumption that there are no more than 3/4 different frames at a time.
     * They will be then assembled like this: (memory_tag_frame_<type> + 0x100llu * (u64)frame_idx),
     * where frame_idx is a value cycling from 0 to AMW_MAX_FRAMES-1. */
    memory_tag_frame_simulation = 0x00001000llu,            /**< End of simulation stage of a frame index. */
    memory_tag_frame_rendering  = 0x00001400llu,            /**< End of rendering stage of a frame index. */
    memory_tag_frame_gpuexec    = 0x00001800llu,            /**< End of GPU execution stage of a frame index. */
    memory_tag_frame_shared     = 0x00001c00llu,            /**< Shared data with a lifetime of a single frame index. */

    memory_tag_custom_mask      = 0x0000000F00000000llu,    /**< Use this mask to ensure your custom application tags will not colide with some internal engine stuff. */
    memory_tag_invalid          = ULONG_LONG_MAX,           /**< Not a valid ownership, indicates an error. */
};

/** Defines a memory block used in ipomoea, the tagged heap allocator. */
struct tagged_heap_node {
    /* TODO */
    uint8_t                    *data;
    struct tagged_heap_node    *left; 
    struct tagged_heap_node    *right; 
    at_u64_t                    tag;    
    at_size_t                   offset;
};

/** TODO My idea with this custom allocator, is to introduce a way to tag 
 *  memory lifetime, preallocate blocks of large-page sized memory (usually 
 *  2MiB per block, owning a tag or a collection of smaller chunks with their 
 *  own tags) and manage all my dynamic memory allocations from this one core 
 *  structure - so in practice, to get rid of any system memory allocations 
 *  during the game loop, and allocate all memory needed upfront. Any other 
 *  memory allocators (like an arena/pool, free-list, stack, etc.) will be 
 *  implemented on top of this core structure, and will make no direct mallocs
 *  themself. Of course i must make sure this allocator is save for fibers.
 *  It's all about managing lifetimes. */
struct ipomoea {
    /* TODO */
    struct tagged_heap_node *root;
};

/** Initializes the tagged heap allocator with given parameters. The allocator will be 
 *  prepared with iafini() before any initialization happens, so all memory ends up cleared. */
AMWAPI i32 iainit(struct ipomoea *ia, thread_t *threads, size_t thread_count);

/** Frees all system memory tied to this tagged heap, clears the allocator. */
AMWAPI void iafini(struct ipomoea *ia);

/** Returns a pointer to a chunk of memory within a block with a given tag. 
 *  If no block with such tag currently exists, the next free block becomes tagged.
 *  If alignment is not 0, the allocation will be additionally aligned to such.
 *  If size is 0, no allocation is done. If can't find an empty and large enough 
 *  memory block, a new one is allocated. If size is over 2 MiB (or another value 
 *  for the system large page size), a new continuous block of memory (of multiple
 *  size of a large page) is allocated, and as an exception, will be freed back to 
 *  the system on iafree(tag) - an 'used' value of over 1 page size indicates this. */
AMWAPI void *ialloc(struct ipomoea *ia, size_t n, size_t alignment, u64 tag);

/** Resets the memory pools of every block that shares this tag. These blocks 
 *  become free and can become owned by another tag, from now on. If no blocks with 
 *  such tag exist, no modifications/writes will be made. */
AMWAPI void iafree(struct ipomoea *ia, u64 tag);

AMW_C_DECL_END
#endif /* _AMW_IPOMOEA_H */
