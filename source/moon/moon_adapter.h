#pragma once

/** @file moon/moon_adapter.h
 *  @brief Rendering backend access points and core defines.
 *
 *  Rendering resources (buffers, textures, texture views, samplers, acceleration structures) 
 *  are represented as IDs and accessed via a bindless API. One ID can be shared between
 *  multiple devices to describe the exact same GPU resource. We define handles for these 
 *  resources with general type safety.
 */
#include "../lake/inthelungs.h"

/** Implements the rendering interface via a graphics API. */
LAKE_DECL_HANDLE(moon_adapter);
/** A rendering device is used to create GPU resources and execute rendering commands. */
LAKE_DECL_HANDLE(moon_device);
/** Represents a block of memory on a physical device (most often VRAM or mapped host memory).
 *  It can be passed into procedures used to create GPU resources to reuse memory blocks. */
LAKE_DECL_HANDLE(moon_memory_heap);
/** Query pools are used to gather GPU execution statistics, like timestamps, occlusion queries 
 *  or pipelines stats - counting how many vertices, primitives, or fragments were processed, etc. 
 *  They are important for profiling, visibility queries and conditional rendering. */
LAKE_DECL_HANDLE(moon_timeline_query_pool);
/** Represents an atomic counter on the GPU used for synchronization on an ever incrementing timeline. */
LAKE_DECL_HANDLE(moon_timeline_semaphore);
/** Represents a binary synchronization primitive where signals and waits must always happen on 1:1 pairs. */
LAKE_DECL_HANDLE(moon_binary_semaphore);
/** The swapchain is a way to present rendered images into a window surface, 
 *  that is owned by the display backend. */
LAKE_DECL_HANDLE(moon_swapchain);
/** Represents pipelines that dispatch compute shaders. */
LAKE_DECL_HANDLE(moon_compute_pipeline);
/** Represents a traditional shader pipeline, may include mesh shaders on hardware support. */
LAKE_DECL_HANDLE(moon_raster_pipeline);
/** A shader pipeline that uses hardware-accelerated ray tracing cores,
 *  enables the use of dedicated ray tracing shaders. */
LAKE_DECL_HANDLE(moon_ray_tracing_pipeline);
/** Work graphs enable shaders to create and schedule GPU work. Experimental hardware feature. */
LAKE_DECL_HANDLE(moon_work_graph_pipeline);
/** An executable command buffer that can be submited into a device queue. Has no public interface. */
LAKE_DECL_HANDLE(moon_executable_commands);

typedef struct { u64 handle; } moon_buffer_id;
typedef struct { u64 handle; } moon_texture_id;
typedef struct { u64 handle; } moon_texture_view_id;
typedef struct { u64 handle; } moon_sampler_id;
typedef struct { u64 handle; } moon_tlas_id;
typedef struct { u64 handle; } moon_blas_id;

#define MOON_HANDLE_INDEX_MASK      ((1ull << 20) - 1)
#define MOON_HANDLE_VERSION_SHIFT   20

#define moon_handle_make(T, IDX, VER) \
    ((T){ .handle = ((u64)(VER) << MOON_HANDLE_VERSION_SHIFT) | ((IDX) & MOON_HANDLE_INDEX_MASK) })
#define moon_handle_get_index(ID) \
    ((u32)((ID).handle & MOON_HANDLE_INDEX_MASK))
#define moon_handle_get_version(ID) \
    ((u64)(((ID).handle) >> MOON_HANDLE_VERSION_SHIFT))
#define moon_handle_set_index(ID, IDX) \
    ((ID).handle = ((ID).handle & ~MOON_HANDLE_INDEX_MASK) | ((IDX) & MOON_HANDLE_INDEX_MASK))
#define moon_handle_set_version(ID, VER) \
    ((ID).handle = ((ID).handle & MOON_HANDLE_INDEX_MASK) | ((u64)(VER) << MOON_HANDLE_VERSION_SHIFT))

/** Represents a raw pointer in device memory. */
typedef u64 moon_device_address;

/** Represents a raw pointer in either device or host memory. */
typedef union {
    moon_device_address         device_address;
    void const                 *host_address;
} moon_device_or_host_address;

#define MOON_MAX_COMPUTE_QUEUE_COUNT            8
#define MOON_MAX_TRANSFER_QUEUE_COUNT           2
#define MOON_MAX_SPARSE_BINDING_QUEUE_COUNT     1
#define MOON_MAX_VIDEO_DECODE_QUEUE_COUNT       1
#define MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT       1

#define MOON_QUEUE_MAIN_BEGIN_INDEX             0
#define MOON_QUEUE_COMPUTE_BEGIN_INDEX          1
#define MOON_QUEUE_TRANSFER_BEGIN_INDEX         (MOON_MAX_COMPUTE_QUEUE_COUNT)
#define MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX   (MOON_QUEUE_TRANSFER_BEGIN_INDEX + MOON_MAX_TRANSFER_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX     (MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX + MOON_MAX_SPARSE_BINDING_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX     (MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX + MOON_MAX_VIDEO_DECODE_QUEUE_COUNT)
#define MOON_QUEUE_INDEX_COUNT                  (MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX + MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT)

typedef enum : s8  {
    moon_queue_type_none            = 0,
    moon_queue_type_main            = (1 << 0),
    moon_queue_type_compute         = (1 << 1),
    moon_queue_type_transfer        = (1 << 2),
    moon_queue_type_any             = moon_queue_type_main | moon_queue_type_compute | moon_queue_type_transfer,
    moon_queue_type_sparse_binding  = (1 << 3),
    moon_queue_type_video_decode    = (1 << 4),
    moon_queue_type_video_encode    = (1 << 5),
    moon_queue_type_count           = 6,
    moon_queue_type_true_any        = moon_queue_type_any | moon_queue_type_sparse_binding | 
                                      moon_queue_type_video_decode | moon_queue_type_video_encode,
} moon_queue_type;

/** Used to access a specific command queue. */
typedef struct {
    moon_queue_type type;
    s8              index;
} moon_queue;

static constexpr moon_queue MOON_QUEUE_MAIN = {moon_queue_type_main, MOON_QUEUE_MAIN_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_COMPUTE_0 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 0};
static constexpr moon_queue MOON_QUEUE_COMPUTE_1 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 1};
static constexpr moon_queue MOON_QUEUE_COMPUTE_2 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 2};
static constexpr moon_queue MOON_QUEUE_COMPUTE_3 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 3};
static constexpr moon_queue MOON_QUEUE_COMPUTE_4 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 4};
static constexpr moon_queue MOON_QUEUE_COMPUTE_5 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 5};
static constexpr moon_queue MOON_QUEUE_COMPUTE_6 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 6};
static constexpr moon_queue MOON_QUEUE_COMPUTE_7 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 7};
static constexpr moon_queue MOON_QUEUE_TRANSFER_0 = {moon_queue_type_transfer, MOON_QUEUE_TRANSFER_BEGIN_INDEX + 0};
static constexpr moon_queue MOON_QUEUE_TRANSFER_1 = {moon_queue_type_transfer, MOON_QUEUE_TRANSFER_BEGIN_INDEX + 1};
static constexpr moon_queue MOON_QUEUE_SPARSE_BINDING = {moon_queue_type_sparse_binding, MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_VIDEO_DECODE = {moon_queue_type_video_decode, MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_VIDEO_ENCODE = {moon_queue_type_video_encode, MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX};

/** Details to assemble a rendering adapter. */
typedef struct {
    lake_app_desc const *app_desc;
} moon_adapter_assembly;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MOON_D3D12
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(moon_adapter_assembly_d3d12, moon_adapter_assembly, moon_adapter);
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(moon_adapter_assembly_metal, moon_adapter_assembly, moon_adapter);
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(moon_adapter_assembly_webgpu, moon_adapter_assembly, moon_adapter);
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(moon_adapter_assembly_vulkan, moon_adapter_assembly, moon_adapter);
#endif /* MOON_VULKAN */

/** A mock rendering adapter, used for debug testing and spec validation. */
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(moon_adapter_assembly_mock, moon_adapter_assembly, moon_adapter);

/** Returns an array of native rendering adapters, with a predefined priority. */
LAKE_NODISCARD LAKE_NONNULL_ALL
LAKEAPI PFN_lake_interface_assembly *LAKECALL
moon_assembly_list_of_native_adapters(
    u32 *out_assembly_count, 
    bool include_fallback);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** Helper to declare interface procedures related to object assembly. TODO allocator. */
#define PFN_MOON_ASSEMBLY(T, Tasm, parent) \
    typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_##Tasm)( \
        moon_##parent parent, moon_##Tasm const *assembly, moon_##T *out_##T)
#define FN_MOON_ASSEMBLY(backend, T, Tasm, parent) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_##Tasm( \
        moon_##parent parent, moon_##Tasm const *assembly, moon_##T *out_##T)

/** Helper to declare interface procedures related to object destruction. */
#define PFN_MOON_ZERO_REFCNT(T) \
    PFN_LAKE_WORK(PFN_moon_##T##_zero_refcnt, moon_##T T)
#define FN_MOON_ZERO_REFCNT(backend, T) \
    FN_LAKE_WORK(_moon_##backend##_##T##_zero_refcnt, moon_##T T)
