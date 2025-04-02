#include "vk_reznor.h"

FN_REZNOR_ASSEMBLY(vulkan, query_pool) {
    assert_debug(work && work->assembly.device_memory && work->memory.data && work->type == reznor_resource_type_query_pool && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->query_pool_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, query_pool) {
    assert_debug(query_pool && query_pool->header.device);
    struct reznor_device *device = query_pool->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->query_pool_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(query_pool);
}
