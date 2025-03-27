#include "vk_reznor.h"

void vulkan_buffer_destroy(struct reznor_buffer *restrict buffer) 
{
    (void)buffer;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, buffer) 
{
    (void)work;
}

void vulkan_bottom_level_destroy(struct reznor_bottom_level *restrict bottom_level)
{
    (void)bottom_level;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, bottom_level)
{
    (void)work;
}

void vulkan_top_level_destroy(struct reznor_top_level *restrict top_level)
{
    (void)top_level;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, top_level)
{
    (void)work;
}
