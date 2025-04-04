#ifdef REZNOR_VULKAN
#include "vk_reznor.h"

static char g_vulkan_pipeline_cache_header_magic[4] = "AMWE";
static u64 g_vulkan_pipeline_cache_header_version = 3ull;

struct vulkan_pipeline_cache_header {
    u32 magic;              /**< An arbitrary magic header to make sure this is actually our cache file. */
    u32 data_size;          /**< Equal to *pDataSize returned by vkGetPipelineCacheData. */
    u64 header_version;     /**< Random seed vulkan_pipeline_cache_header_version. */
    u64 data_hash;          /**< A hash of pipeline cache data, including the header. */

    u32 vendor_id;          /**< Equal to VkPhysicalDeviceProperties::vendorID. */
    u32 device_id;          /**< Equal to VkPhysicalDeviceProperties::deviceID. */
    u32 driver_version;     /**< Equal to VkPhysicalDeviceProperties::driverVersion. */
    u32 driver_abi;         /**< Equal to sizeof(void*). */

    u8  uuid[VK_UUID_SIZE]; /**< Equal to VkPhysicalDeviceProperties::pipelineCacheUUID */
};

FN_REZNOR_ASSEMBLY(vulkan, pipeline_layout) 
{
    assert_debug(work && work->assembly.pipeline_layout && work->memory.data && work->type == reznor_resource_type_pipeline_layout && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->pipeline_layout_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, pipeline_layout) 
{
    assert_debug(pipeline_layout && pipeline_layout->header.device);
    struct reznor_device *device = pipeline_layout->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->pipeline_layout_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_ASSEMBLY(vulkan, graphics_pipeline) 
{
    assert_debug(work && work->assembly.graphics_pipeline && work->memory.data && work->type == reznor_resource_type_graphics_pipeline && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->graphics_pipeline_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, graphics_pipeline) 
{
    assert_debug(graphics_pipeline && graphics_pipeline->header.device);
    struct reznor_device *device = graphics_pipeline->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->graphics_pipeline_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_ASSEMBLY(vulkan, compute_pipeline) 
{
    assert_debug(work && work->assembly.compute_pipeline && work->memory.data && work->type == reznor_resource_type_compute_pipeline && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->compute_pipeline_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, compute_pipeline) 
{
    assert_debug(compute_pipeline && compute_pipeline->header.device);
    struct reznor_device *device = compute_pipeline->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->compute_pipeline_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_ASSEMBLY(vulkan, raytracing_pipeline) 
{
    assert_debug(work && work->assembly.raytracing_pipeline && work->memory.data && work->type == reznor_resource_type_raytracing_pipeline && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->raytracing_pipeline_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, raytracing_pipeline) 
{
    assert_debug(raytracing_pipeline && raytracing_pipeline->header.device);
    struct reznor_device *device = raytracing_pipeline->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->raytracing_pipeline_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

#endif /* REZNOR_VULKAN */
