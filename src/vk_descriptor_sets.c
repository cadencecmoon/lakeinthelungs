#include "vk_reznor.h"

void vulkan_descriptor_set_layout_destroy(struct reznor_descriptor_set_layout *restrict descriptor_set_layout)
{
    (void)descriptor_set_layout;
}

void vulkan_descriptor_set_destroy(struct reznor_descriptor_set *restrict descriptor_set)
{
    (void)descriptor_set;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, descriptor_set_layout)
{
    (void)device;
    (void)descriptor_set_layout_count;
    (void)descriptor_set_layout_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, descriptor_set)
{
    (void)device;
    (void)descriptor_set_count;
    (void)descriptor_set_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}
