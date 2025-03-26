#include "vk_reznor.h"

FN_REZNOR_DEVICE_QUERY(vulkan)
{
    (void)reznor;
    (void)out_device_count;
    (void)out_devices;
    return result_error;
}

FN_REZNOR_DEVICE_CREATE(vulkan)
{
    (void)reznor;
    (void)config;
    (void)memory;
    (void)out_device;
    return result_error;
}

FN_REZNOR_DEVICE_DESTROY(vulkan)
{
    (void)device;
}

FN_REZNOR_FRAME_BEGIN(vulkan)
{
    (void)frame;
}

FN_REZNOR_FRAME_SUBMIT(vulkan)
{
    (void)frame;
}

static const PFN_riven_work disassembly_dispatch_table[] = {
    riven_work_nop,
    (PFN_riven_work)vulkan_device_memory_destroy,
    (PFN_riven_work)vulkan_buffer_destroy,
    (PFN_riven_work)vulkan_texture_destroy,
    (PFN_riven_work)vulkan_sampler_destroy,
    (PFN_riven_work)vulkan_descriptor_set_layout_destroy,
    (PFN_riven_work)vulkan_descriptor_set_destroy,
    (PFN_riven_work)vulkan_pipeline_layout_destroy,
    (PFN_riven_work)vulkan_graphics_pipeline_destroy,
    (PFN_riven_work)vulkan_compute_pipeline_destroy,
    (PFN_riven_work)vulkan_raytracing_pipeline_destroy,
    (PFN_riven_work)vulkan_shader_binding_table_destroy,
    (PFN_riven_work)vulkan_bottom_level_destroy,
    (PFN_riven_work)vulkan_top_level_destroy,
    (PFN_riven_work)vulkan_query_pool_destroy,
    (PFN_riven_work)vulkan_swapchain_destroy,
};

FN_REZNOR_DISSASEMBLY(vulkan)
{
    for (u32 i = 0; i < resource_count; i++) {
        struct reznor_resource_header *header = (struct reznor_resource_header *)resources[i];
        assert_debug(header->type != reznor_resource_type_invalid && header->type < reznor_resource_type_count);
        disassembly_dispatch_table[header->type]((riven_argument_t)header);
    }
}
