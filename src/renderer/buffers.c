/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/pelagia.h>
#include <lake/vulkan.h>

AMWAPI void vulkan_destroy_buffers(
    struct vulkan_buffers *buffers, 
    struct vulkan_device  *device,
    struct vulkan_backend *vk)
{
    (void)buffers;
    (void)device;
    (void)vk;
}

AMWAPI s32 vulkan_create_aligned_buffers(
    struct vulkan_buffers                 *buffers,
    struct vulkan_device                  *device,
    const VkBufferCreateInfo              *buffer_infos,
    u32                                    buffer_count,
    const struct vulkan_allocation_request allocation_request,
    VkDeviceSize                           alignment)
{
    (void)buffers;
    (void)device;
    (void)buffer_infos;
    (void)buffer_count;
    (void)allocation_request;
    (void)alignment;
    return result_error_undefined;
}

AMWAPI s32 vulkan_copy_buffers_and_images(
    const struct vulkan_device *device,
    VkCommandBuffer             command_buffer,
    u32                         buffer_count, 
    const VkBuffer             *source_buffers,
    const VkBuffer             *destination_buffers,
    VkBufferCopy               *buffer_regions,
    u32                         image_count,
    const VkImage              *source_images,
    const VkImage              *destination_images,
    VkImageLayout               source_layout,
    VkImageLayout               destination_layout_before,
    VkImageLayout               destination_layout_after,
    VkImageCopy                *image_regions,
    u32                         buffer_to_image_count,
    const VkBuffer             *image_source_buffers,
    const VkImage              *buffer_destination_images,
    VkImageLayout               buffer_destination_layout_before,
    VkImageLayout               buffer_destination_layout_after,
    VkBufferImageCopy          *buffer_to_image_regions)
{
    (void)device;
    (void)command_buffer;
    (void)buffer_count;
    (void)source_buffers;
    (void)destination_buffers;
    (void)buffer_regions;
    (void)image_count;
    (void)source_images;
    (void)destination_images;
    (void)source_layout;
    (void)destination_layout_before;
    (void)destination_layout_after;
    (void)image_regions;
    (void)buffer_to_image_count;
    (void)image_source_buffers;
    (void)buffer_destination_images;
    (void)buffer_destination_layout_before;
    (void)buffer_destination_layout_after;
    (void)buffer_to_image_regions;
    return result_error_undefined;
}

AMWAPI void pelagia_assemble_uniform_buffers(struct pelagia_assemble_uniform_buffers_work *work)
{
    PELAGIA_WORK_INITIALIZED_OR_RETURN

    struct pelagia         *pelagia         = work->pelagia;
    struct vulkan_buffers  *uniform_buffers = &pelagia->uniform_buffers;
    struct vulkan_device   *device          = &pelagia->device;
    struct vulkan_backend  *vk              = &pelagia->vk;
    struct arena_allocator *scratch_arena   = &pelagia->scratch_arena;

    /* cleanup the uniform buffers */
    vulkan_destroy_buffers(uniform_buffers, device, vk);
    if (work->dissasemble) return;

    (void)uniform_buffers;
    (void)scratch_arena;

    /* TODO */

    work->out_result = result_error_undefined; /* TODO */
}
