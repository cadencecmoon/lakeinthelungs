#pragma once

/** @file lake/bedrock/tagged_heap.h 
 *  @brief TODO docs
 */
#include "types.h"
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** We assume a block size of 256KB. A range of 8 blocks is equal to a hugepage. */
#define LAKE_HEAP_BLOCK_SIZE (256lu*1024lu)

/** Align an offset to a tagge heap block. */
#define lake_align_heap_block(V) lake_align(V, LAKE_HEAP_BLOCK_SIZE)
/** Align to a range of 8 blocks (a common hugepage size). */
#define lake_align_heap_hugepage(V) lake_align(V, LAKE_HEAP_BLOCK_SIZE)

/** A heap tag describes an unique lifetime. Resources of the same lifetime 
 *  are grouped together, so later they can be released together all at once.
 *  The tagged heap is the main strategy to allocate system resources and 
 *  serves as a backend for custom allocator data structures. */
typedef u32 lake_heap_tag;

/* These are ensured to be a power of 2. */
#define lake_heap_tag_scratch_cycle 8
#define lake_heap_tag_instance_arrays_cycle 4
#define lake_heap_tag_skinning_matrices_cycle 4
#define lake_heap_tag_command_buffers_cycle 4

/** Reserved tags for expected frequencies of resource lifetime. */
enum : lake_heap_tag {
    /** Resources under this tag cannot ever be freed. They will share the 
     *  lifetime of the framework - check `lake_in_the_lungs()`. */
    lake_heap_tag_roots = 0u,
    /** A single frequency for logger data, reset every time a log flush job is submitted. */
    lake_heap_tag_logger,
    /** Frequencies used for different stages within the gameloop pipeline. This tag is cycled 
     *  at the beginning of every new frame, and a stage in the pipeline is the owner of the tag, 
     *  guaranteeing that any parallel work outside of this stage does not use this tag. These tags 
     *  CANNOT be used to feed forward data between stages or frames, it's purpose is scratch memory
     *  for resources that should life for the duration of a gameloop pipeline stage. */
    lake_heap_tag_scratch,
    /** Frequencies used for instance arrays: gameplay -> rendering. */
    lake_heap_tag_instance_arrays = lake_heap_tag_scratch + lake_heap_tag_scratch_cycle,
    /** Frequencies used for skinning matrices: gameplay -> GPU exec. */
    lake_heap_tag_skinning_matrices = lake_heap_tag_instance_arrays + lake_heap_tag_instance_arrays_cycle,
    /** Frequencies used for command buffers: rendering -> GPU exec. */
    lake_heap_tag_command_buffers = lake_heap_tag_skinning_matrices + lake_heap_tag_skinning_matrices_cycle,
    /** Count of all reserved heap tags. */
    lake_heap_tag_max_enum = lake_heap_tag_command_buffers + lake_heap_tag_command_buffers_cycle,
};

/** Describes the purpose of a memory allocation. Because the tagged heap does not keep track of 
 *  suballocations within a block of memory, the whole block range gets an attribute assigned to it.
 *  The attributes are used for debugging and profiling memory allocations. */
typedef enum : s8 {
    lake_heap_attr_invalid = -1,    /**< A memory block is invalid, can be ignored for profiling. */
    lake_heap_attr_default = 0,     /**< The purpose of this memory allocation was not really specified. */
    lake_heap_attr_animation,
    lake_heap_attr_world,
    lake_heap_attr_gameplay,
    lake_heap_attr_fx,
    lake_heap_attr_render,
    lake_heap_attr_physics,
    lake_heap_attr_audio,
    lake_heap_attr_network,
    lake_heap_attr_system,
    lake_heap_attr_script,
    lake_heap_attr_resource,
    lake_heap_attr_ui,
    lake_heap_attr_debug,
    lake_heap_attr_max_enum,
} lake_heap_attr;

/** Details of an allocation request acquired from the tagged heap. 
 *  Can be used to initialize custom memory allocators. */
typedef struct {
    /** Points to the beginning of this allocation. */
    u8           *data;
    /** A multiple of `LAKE_HEAP_BLOCK_SIZE`, the memory blocks are contiguous. */
    usize         size;
    /** The tagged heap used for this allocation. */
    lake_heap_tag tag;
    /** Allows a custom allocator to hook this memory range for detailed profiling. */
    u32           id;
} lake_heap;

/** Allocates a block (or range of blocks) of memory under this tag. The requested 
 *  size will be aligned to `LAKE_HEAP_BLOCK_SIZE`. Requests of size 0 are ignored. 
 *  Writes details of the allocation to an output `lake_heap_memory` structure, and the 
 *  user is responsible for managing this memory block. To free resources, it is 
 *  enough to call `lake_theap_free(tag)` with the `tag` used for this request.
 *
 *  @return LAKE_SUCCESS, LAKE_ERROR_OUT_OF_HOST_MEMORY, LAKE_FRAMEWORK_REQUIRED. */
LAKE_NODISCARD LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI lake_result LAKECALL 
lake_theap_alloc(
    lake_heap_tag   tag, 
    lake_heap_attr  attr,
    usize           request, 
    lake_heap      *out_heap);

/** Releases all resources that were requested under this tag, so the memory heaps 
 *  can be reused with new requests.
 *
 *  @return LAKE_SUCCESS, LAKE_FRAMEWORK_REQUIRED. */
LAKE_NODISCARD LAKE_HOT_FN
LAKEAPI lake_result LAKECALL 
lake_theap_free(
    lake_heap_tag tag);

/** Advises the commitment of physical resources used by internal tagged heaps.
 *  All requests are aligned to a hugepage boundary (8 memory blocks of 256KB). 
 *
 *  The outcome for growth depends on framework limits set at initialization,
 *  and on the current state of the host system, and available resources. 
 *
 *  The outcome for release depends on internal fragmentation of blocks and 
 *  total resource use within the commited memory range.
 *
 *  @return LAKE_SUCCESS, LAKE_SUBOPTIMAL, LAKE_ERROR_OUT_OF_HOST_MEMORY, 
 *      LAKE_OPERATION_DEFERRED, LAKE_FRAMEWORK_REQUIRED. */
LAKE_NODISCARD
LAKEAPI lake_result LAKECALL 
lake_thadvise_heap(
    usize             request, 
    lake_madvise_mode mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
