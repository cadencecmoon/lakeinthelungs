#include "vk_reznor.h"

FN_REZNOR_COMMAND_DRAW(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)vertex_count;
    (void)instance_count;
    (void)vertex_offset;
    (void)instance_offset;
}

FN_REZNOR_COMMAND_DRAW_INDEXED(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)index_count;
    (void)instance_count;
    (void)index_offset;
    (void)vertex_offset;
    (void)instance_offset;
}

FN_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)buffer;
    (void)offset;
    (void)draw_count;
    (void)stride;
}

FN_REZNOR_COMMAND_DRAW_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)buffer;
    (void)offset;
    (void)draw_count;
    (void)stride;
}

FN_REZNOR_COMMAND_DISPATCH(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)group_count_x;
    (void)group_count_y;
    (void)group_count_z;
}

FN_REZNOR_COMMAND_DISPATCH_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)buffer;
    (void)offset;
}

FN_REZNOR_COMMAND_COPY_BUFFER(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)regions;
    (void)region_count;
    (void)dst_buffer;
    (void)src_buffer;
}

FN_REZNOR_COMMAND_COPY_TEXTURE(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)regions;
    (void)region_count;
    (void)dst_texture;
    (void)src_texture;
}

FN_REZNOR_COMMAND_BEGIN_RENDER_PASS(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
    (void)params;
}

FN_REZNOR_COMMAND_END_RENDER_PASS(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    (void)device;
}
