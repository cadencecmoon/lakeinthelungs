#pragma once

/** @file moon/moon_memory_heap.h
 *  @brief Represents memory in a GPU.
 *
 *  Following interface procedures are defined:
 *      PFN_moon_memory_heap_assembly
 *      PFN_moon_memory_heap_zero_refcnt
 */
#include "moon_adapter.h"

typedef u8 moon_memory_flags;
typedef enum : moon_memory_flags {
    moon_memory_flag_none                           = 0,
    moon_memory_flag_dedicated_memory               = (1u << 0),
    moon_memory_flag_can_alias                      = (1u << 1),
    moon_memory_flag_host_access_sequential_write   = (1u << 2),
    moon_memory_flag_host_access_random             = (1u << 3),
    moon_memory_flag_strategy_min_memory            = (1u << 4),
    moon_memory_flag_strategy_min_time              = (1u << 5),
} moon_memory_flag_bits;

typedef struct {
    u64                         size;
    u64                         alignment;
    u32                         type_bitmask; /** Bitmask of indices into `moon_device_details->memory_types[]`. */
} moon_memory_requirements;

typedef u8 moon_memory_heap_property; 
typedef enum : moon_memory_heap_property {
    moon_memory_property_none                                           = 0,
    moon_memory_property_device_local                                   = (1u << 0), 
    moon_memory_property_device_coherent                                = (1u << 1), 
    moon_memory_property_device_uncached                                = (1u << 2), 
    moon_memory_property_host_visible                                   = (1u << 3), 
    moon_memory_property_host_coherent                                  = (1u << 4), 
    moon_memory_property_host_cached                                    = (1u << 5), 
    moon_memory_property_lazily_allocated                               = (1u << 6), 
    moon_memory_property_protected                                      = (1u << 7), 
} moon_memory_heap_property_bits;

typedef struct {
    moon_memory_heap_property   property;
    u8                          heap_index;
} moon_memory_heap_type;
#define MOON_MAX_MEMORY_HEAP_TYPES 32

typedef struct {
    moon_memory_requirements        requirements;
    moon_memory_flags               flags;
} moon_memory_heap_assembly;
static constexpr moon_memory_heap_assembly MOON_MEMORY_HEAP_ASSEMBLY_INIT = {0};

typedef struct {
    moon_buffer_id                  buffer;
    u64                             size : 63;
    u64                             heap_allocated : 1;
} moon_memory_buffer_size_pair;

typedef struct {
    moon_texture_id                 texture;
    u64                             size : 63;
    u64                             heap_allocated : 1;
} moon_memory_texture_size_pair;

typedef struct {
    moon_tlas_id                    tlas;
    u64                             size;
} moon_memory_tlas_size_pair;

typedef struct {
    moon_blas_id                    blas;
    u64                             size;
} moon_memory_blas_size_pair;

typedef struct {
    moon_memory_heap                heap;
    u64                             size;
} moon_memory_heap_size_pair;

typedef struct {
    u64                             total_memory_use;
    u64                             total_buffer_memory_use;
    u64                             total_texture_memory_use;
    u64                             total_aliased_tlas_memory_use;
    u64                             total_aliased_blas_memory_use;
    u64                             total_device_memory_use;
    u32                             buffer_count;
    u32                             texture_count;
    u32                             tlas_count;
    u32                             blas_count;
    u32                             memory_count;
    moon_memory_buffer_size_pair   *buffer_list;
    moon_memory_texture_size_pair  *texture_list;
    moon_memory_tlas_size_pair     *tlas_list;
    moon_memory_blas_size_pair     *blas_list;
    moon_memory_heap_size_pair     *memory_list;
} moon_memory_report;

PFN_MOON_ASSEMBLY(memory_heap, memory_heap_assembly, device);
#define FN_MOON_MEMORY_HEAP_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, memory_heap, memory_heap_assembly, device)

PFN_MOON_ZERO_REFCNT(memory_heap);
#define FN_MOON_MEMORY_HEAP_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, memory_heap)
