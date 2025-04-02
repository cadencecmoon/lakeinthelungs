#include "vk_reznor.h"

FN_REZNOR_ASSEMBLY(vulkan, descriptor_set_layout) {
    assert_debug(work && work->assembly.descriptor_set_layout && work->memory.data && work->type == reznor_resource_type_descriptor_set_layout && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->descriptor_set_layout_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, descriptor_set_layout) {
    assert_debug(descriptor_set_layout && descriptor_set_layout->header.device);
    struct reznor_device *device = descriptor_set_layout->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->descriptor_set_layout_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(descriptor_set_layout);
}

FN_REZNOR_ASSEMBLY(vulkan, descriptor_set) {
    assert_debug(work && work->assembly.descriptor_set && work->memory.data && work->type == reznor_resource_type_descriptor_set && work->device);
    struct reznor_device *device = work->device;

    (void)device;
}

FN_REZNOR_DISASSEMBLY(vulkan, descriptor_set) {
    assert_debug(descriptor_set && descriptor_set->header.device);
    struct reznor_device *device = descriptor_set->header.device;

    (void)device;

    zerop(descriptor_set);
}
