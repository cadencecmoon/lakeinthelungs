#pragma once

/** @file moon/moon_timeline.h
 *  @brief Primitives for synchronization and timeline query.
 *
 *  Following interface procedures are defined:
 *      PFN_moon_timeline_query_pool_assembly
 *      PFN_moon_timeline_query_pool_zero_refcnt
 *      PFN_moon_timeline_semaphore_assembly
 *      PFN_moon_timeline_semaphore_zero_refcnt
 *      PFN_moon_binary_semaphore_assembly
 *      PFN_moon_binary_semaphore_zero_refcnt
 */
#include "moon_adapter.h"
#include "moon_gpu_resources.h"

typedef u32 moon_pipeline_stages;
typedef enum : moon_pipeline_stages {
    moon_pipeline_stage_none                                = 0u,
    moon_pipeline_stage_top_of_pipe                         = (1u << 0),
    moon_pipeline_stage_draw_indirect                       = (1u << 1),
    moon_pipeline_stage_vertex_shader                       = (1u << 2),
    moon_pipeline_stage_tesselation_control_shader          = (1u << 3),
    moon_pipeline_stage_tesselation_evaluation_shader       = (1u << 4),
    moon_pipeline_stage_geometry_shader                     = (1u << 5),
    moon_pipeline_stage_fragment_shader                     = (1u << 6),
    moon_pipeline_stage_early_fragment_tests                = (1u << 7),
    moon_pipeline_stage_late_fragment_tests                 = (1u << 8),
    moon_pipeline_stage_color_attachment_output             = (1u << 9),
    moon_pipeline_stage_compute_shader                      = (1u << 10),
    moon_pipeline_stage_transfer                            = (1u << 11),
    moon_pipeline_stage_bottom_of_pipe                      = (1u << 12),
    moon_pipeline_stage_host                                = (1u << 13),
    moon_pipeline_stage_all_graphics                        = (1u << 14),
    moon_pipeline_stage_all_commands                        = (1u << 15),
    moon_pipeline_stage_copy                                = (1u << 16),
    moon_pipeline_stage_resolve                             = (1u << 17),
    moon_pipeline_stage_blit                                = (1u << 18),
    moon_pipeline_stage_clear                               = (1u << 19),
    moon_pipeline_stage_index_input                         = (1u << 20),
    moon_pipeline_stage_pre_rasterization_shaders           = (1u << 21),
    moon_pipeline_stage_task_shader                         = (1u << 22),
    moon_pipeline_stage_mesh_shader                         = (1u << 23),
    moon_pipeline_stage_acceleration_structure_build        = (1u << 24),
    moon_pipeline_stage_ray_tracing_shader                  = (1u << 25),
} moon_pipeline_stage_bits;

typedef u32 moon_access_type;
typedef enum : moon_access_type {
    moon_access_type_none       = 0u,
    moon_access_type_read       = (1u << 31),
    moon_access_type_write      = (1u << 30),
    moon_access_type_read_write = moon_access_type_read | moon_access_type_write,
} moon_access_type_bits;

/* moon_pipeline_stages + moon_access_type */
typedef u32 moon_access;
#define MOON_ACCESS_MASK_PIPELINE_STAGE(ACCESS) ((ACCESS) & 0x3fffffffu)
#define MOON_ACCESS_MASK_TYPE(ACCESS)           ((ACCESS) & 0xc0000000u)

/** Declares a compile-time access detail for common use cases. */
#define MOON_ACCESS_DECL(NAME, STAGES, TYPE) \
    static constexpr moon_access MOON_ACCESS_##NAME = STAGES | TYPE

MOON_ACCESS_DECL(NONE, moon_pipeline_stage_none, moon_access_type_none);
/* READ ACCESS */
MOON_ACCESS_DECL(TOP_OF_PIPE_READ, moon_pipeline_stage_top_of_pipe, moon_access_type_read);
MOON_ACCESS_DECL(DRAW_INDIRECT_READ, moon_pipeline_stage_draw_indirect, moon_access_type_read);
MOON_ACCESS_DECL(VERTEX_SHADER_READ, moon_pipeline_stage_vertex_shader, moon_access_type_read);
MOON_ACCESS_DECL(TESSELATION_CONTROL_SHADER_READ, moon_pipeline_stage_tesselation_control_shader, moon_access_type_read);
MOON_ACCESS_DECL(TESSELATION_EVALUATION_SHADER_READ, moon_pipeline_stage_tesselation_evaluation_shader, moon_access_type_read);
MOON_ACCESS_DECL(GEOMETRY_SHADER_READ, moon_pipeline_stage_geometry_shader, moon_access_type_read);
MOON_ACCESS_DECL(FRAGMENT_SHADER_READ, moon_pipeline_stage_fragment_shader, moon_access_type_read);
MOON_ACCESS_DECL(EARLY_FRAGMENT_TESTS_READ, moon_pipeline_stage_early_fragment_tests, moon_access_type_read);
MOON_ACCESS_DECL(LATE_FRAGMENT_TESTS_READ, moon_pipeline_stage_late_fragment_tests, moon_access_type_read);
MOON_ACCESS_DECL(COLOR_ATTACHMENT_OUTPUT_READ, moon_pipeline_stage_color_attachment_output, moon_access_type_read);
MOON_ACCESS_DECL(COMPUTE_SHADER_READ, moon_pipeline_stage_compute_shader, moon_access_type_read);
MOON_ACCESS_DECL(TRANSFER_READ, moon_pipeline_stage_transfer, moon_access_type_read);
MOON_ACCESS_DECL(BOTTOM_OF_PIPE_READ, moon_pipeline_stage_bottom_of_pipe, moon_access_type_read);
MOON_ACCESS_DECL(HOST_READ, moon_pipeline_stage_host, moon_access_type_read);
MOON_ACCESS_DECL(ALL_GRAPHICS_READ, moon_pipeline_stage_all_graphics, moon_access_type_read);
MOON_ACCESS_DECL(READ, moon_pipeline_stage_all_commands, moon_access_type_read);
MOON_ACCESS_DECL(COPY_READ, moon_pipeline_stage_copy, moon_access_type_read);
MOON_ACCESS_DECL(RESOLVE_READ, moon_pipeline_stage_resolve, moon_access_type_read);
MOON_ACCESS_DECL(BLIT_READ, moon_pipeline_stage_blit, moon_access_type_read);
MOON_ACCESS_DECL(CLEAR_READ, moon_pipeline_stage_clear, moon_access_type_read);
MOON_ACCESS_DECL(INDEX_INPUT_READ, moon_pipeline_stage_index_input, moon_access_type_read);
MOON_ACCESS_DECL(PRE_RASTERIZATION_SHADERS_READ, moon_pipeline_stage_pre_rasterization_shaders, moon_access_type_read);
MOON_ACCESS_DECL(TASK_SHADER_READ, moon_pipeline_stage_task_shader, moon_access_type_read);
MOON_ACCESS_DECL(MESH_SHADER_READ, moon_pipeline_stage_mesh_shader, moon_access_type_read);
MOON_ACCESS_DECL(ACCELERATION_STRUCTURE_BUILD_READ, moon_pipeline_stage_acceleration_structure_build, moon_access_type_read);
MOON_ACCESS_DECL(RAY_TRACING_SHADER_READ, moon_pipeline_stage_ray_tracing_shader, moon_access_type_read);
/* WRITE ACCESS */
MOON_ACCESS_DECL(TOP_OF_PIPE_WRITE, moon_pipeline_stage_top_of_pipe, moon_access_type_write);
MOON_ACCESS_DECL(DRAW_INDIRECT_WRITE, moon_pipeline_stage_draw_indirect, moon_access_type_write);
MOON_ACCESS_DECL(VERTEX_SHADER_WRITE, moon_pipeline_stage_vertex_shader, moon_access_type_write);
MOON_ACCESS_DECL(TESSELATION_CONTROL_SHADER_WRITE, moon_pipeline_stage_tesselation_control_shader, moon_access_type_write);
MOON_ACCESS_DECL(TESSELATION_EVALUATION_SHADER_WRITE, moon_pipeline_stage_tesselation_evaluation_shader, moon_access_type_write);
MOON_ACCESS_DECL(GEOMETRY_SHADER_WRITE, moon_pipeline_stage_geometry_shader, moon_access_type_write);
MOON_ACCESS_DECL(FRAGMENT_SHADER_WRITE, moon_pipeline_stage_fragment_shader, moon_access_type_write);
MOON_ACCESS_DECL(EARLY_FRAGMENT_TESTS_WRITE, moon_pipeline_stage_early_fragment_tests, moon_access_type_write);
MOON_ACCESS_DECL(LATE_FRAGMENT_TESTS_WRITE, moon_pipeline_stage_late_fragment_tests, moon_access_type_write);
MOON_ACCESS_DECL(COLOR_ATTACHMENT_OUTPUT_WRITE, moon_pipeline_stage_color_attachment_output, moon_access_type_write);
MOON_ACCESS_DECL(COMPUTE_SHADER_WRITE, moon_pipeline_stage_compute_shader, moon_access_type_write);
MOON_ACCESS_DECL(TRANSFER_WRITE, moon_pipeline_stage_transfer, moon_access_type_write);
MOON_ACCESS_DECL(BOTTOM_OF_PIPE_WRITE, moon_pipeline_stage_bottom_of_pipe, moon_access_type_write);
MOON_ACCESS_DECL(HOST_WRITE, moon_pipeline_stage_host, moon_access_type_write);
MOON_ACCESS_DECL(ALL_GRAPHICS_WRITE, moon_pipeline_stage_all_graphics, moon_access_type_write);
MOON_ACCESS_DECL(WRITE, moon_pipeline_stage_all_commands, moon_access_type_write);
MOON_ACCESS_DECL(COPY_WRITE, moon_pipeline_stage_copy, moon_access_type_write);
MOON_ACCESS_DECL(RESOLVE_WRITE, moon_pipeline_stage_resolve, moon_access_type_write);
MOON_ACCESS_DECL(BLIT_WRITE, moon_pipeline_stage_blit, moon_access_type_write);
MOON_ACCESS_DECL(CLEAR_WRITE, moon_pipeline_stage_clear, moon_access_type_write);
MOON_ACCESS_DECL(INDEX_INPUT_WRITE, moon_pipeline_stage_index_input, moon_access_type_write);
MOON_ACCESS_DECL(PRE_RASTERIZATION_SHADERS_WRITE, moon_pipeline_stage_pre_rasterization_shaders, moon_access_type_write);
MOON_ACCESS_DECL(TASK_SHADER_WRITE, moon_pipeline_stage_task_shader, moon_access_type_write);
MOON_ACCESS_DECL(MESH_SHADER_WRITE, moon_pipeline_stage_mesh_shader, moon_access_type_write);
MOON_ACCESS_DECL(ACCELERATION_STRUCTURE_BUILD_WRITE, moon_pipeline_stage_acceleration_structure_build, moon_access_type_write);
MOON_ACCESS_DECL(RAY_TRACING_SHADER_WRITE, moon_pipeline_stage_ray_tracing_shader, moon_access_type_write);
/* READ/WRITE ACCESS */
MOON_ACCESS_DECL(TOP_OF_PIPE_READ_WRITE, moon_pipeline_stage_top_of_pipe, moon_access_type_read_write);
MOON_ACCESS_DECL(DRAW_INDIRECT_READ_WRITE, moon_pipeline_stage_draw_indirect, moon_access_type_read_write);
MOON_ACCESS_DECL(VERTEX_SHADER_READ_WRITE, moon_pipeline_stage_vertex_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(TESSELATION_CONTROL_SHADER_READ_WRITE, moon_pipeline_stage_tesselation_control_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(TESSELATION_EVALUATION_SHADER_READ_WRITE, moon_pipeline_stage_tesselation_evaluation_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(GEOMETRY_SHADER_READ_WRITE, moon_pipeline_stage_geometry_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(FRAGMENT_SHADER_READ_WRITE, moon_pipeline_stage_fragment_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(EARLY_FRAGMENT_TESTS_READ_WRITE, moon_pipeline_stage_early_fragment_tests, moon_access_type_read_write);
MOON_ACCESS_DECL(LATE_FRAGMENT_TESTS_READ_WRITE, moon_pipeline_stage_late_fragment_tests, moon_access_type_read_write);
MOON_ACCESS_DECL(COLOR_ATTACHMENT_OUTPUT_READ_WRITE, moon_pipeline_stage_color_attachment_output, moon_access_type_read_write);
MOON_ACCESS_DECL(COMPUTE_SHADER_READ_WRITE, moon_pipeline_stage_compute_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(TRANSFER_READ_WRITE, moon_pipeline_stage_transfer, moon_access_type_read_write);
MOON_ACCESS_DECL(BOTTOM_OF_PIPE_READ_WRITE, moon_pipeline_stage_bottom_of_pipe, moon_access_type_read_write);
MOON_ACCESS_DECL(HOST_READ_WRITE, moon_pipeline_stage_host, moon_access_type_read_write);
MOON_ACCESS_DECL(ALL_GRAPHICS_READ_WRITE, moon_pipeline_stage_all_graphics, moon_access_type_read_write);
MOON_ACCESS_DECL(READ_WRITE, moon_pipeline_stage_all_commands, moon_access_type_read_write);
MOON_ACCESS_DECL(COPY_READ_WRITE, moon_pipeline_stage_copy, moon_access_type_read_write);
MOON_ACCESS_DECL(RESOLVE_READ_WRITE, moon_pipeline_stage_resolve, moon_access_type_read_write);
MOON_ACCESS_DECL(BLIT_READ_WRITE, moon_pipeline_stage_blit, moon_access_type_read_write);
MOON_ACCESS_DECL(CLEAR_READ_WRITE, moon_pipeline_stage_clear, moon_access_type_read_write);
MOON_ACCESS_DECL(INDEX_INPUT_READ_WRITE, moon_pipeline_stage_index_input, moon_access_type_read_write);
MOON_ACCESS_DECL(PRE_RASTERIZATION_SHADERS_READ_WRITE, moon_pipeline_stage_pre_rasterization_shaders, moon_access_type_read_write);
MOON_ACCESS_DECL(TASK_SHADER_READ_WRITE, moon_pipeline_stage_task_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(MESH_SHADER_READ_WRITE, moon_pipeline_stage_mesh_shader, moon_access_type_read_write);
MOON_ACCESS_DECL(ACCELERATION_STRUCTURE_BUILD_READ_WRITE, moon_pipeline_stage_acceleration_structure_build, moon_access_type_read_write);
MOON_ACCESS_DECL(RAY_TRACING_SHADER_READ_WRITE, moon_pipeline_stage_ray_tracing_shader, moon_access_type_read_write);
#undef MOON_ACCESS_DECL

/** Pipeline barrier used for memory access. */
typedef struct {
    moon_access                     src_access;
    moon_access                     dst_access;
} moon_memory_barrier;

/** Pipeline barrier used for image access and transitions. */
typedef struct {
    moon_memory_barrier             barrier;
    moon_texture_layout             src_layout;
    moon_texture_layout             dst_layout;
    moon_texture_mip_array_slice    texture_slice;
    moon_texture_id                 texture;
} moon_texture_memory_barrier;

/** Details needed to create a timeline query pool. */
typedef struct {
    u32                             query_count;
    lake_small_string               name;
} moon_timeline_query_pool_assembly;
static constexpr moon_timeline_query_pool_assembly MOON_TIMELINE_QUERY_POOL_ASSEMBLY_INIT = {0};

/** Details needed to create a timeline semaphore. */
typedef struct {
    u64                             initial_value;
    lake_small_string               name;
} moon_timeline_semaphore_assembly;
static constexpr moon_timeline_semaphore_assembly MOON_TIMELINE_SEMAPHORE_ASSEMBLY_INIT = {0};

/** Pairs a GPU timeline with a CPU timeline value. */
typedef struct {
    moon_timeline_semaphore         timeline_semaphore;
    u64                             value;
} moon_timeline_pair;

/** Details needed to create a binary semaphore. */
typedef struct {
    lake_small_string               name;
} moon_binary_semaphore_assembly;
static constexpr moon_binary_semaphore_assembly MOON_BINARY_SEMAPHORE_ASSEMBLY_INIT = {0};

PFN_MOON_ASSEMBLY(timeline_query_pool, timeline_query_pool_assembly, device);
#define FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, timeline_query_pool, device)

PFN_MOON_ZERO_REFCNT(timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, timeline_query_pool)

PFN_MOON_ASSEMBLY(timeline_semaphore, timeline_semaphore_assembly, device);
#define FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, timeline_semaphore, device)

PFN_MOON_ZERO_REFCNT(timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, timeline_semaphore)

PFN_MOON_ASSEMBLY(binary_semaphore, binary_semaphore_assembly, device);
#define FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, binary_semaphore, device)

PFN_MOON_ZERO_REFCNT(binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, binary_semaphore)
