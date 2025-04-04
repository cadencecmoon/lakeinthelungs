#ifdef REZNOR_VULKAN
#include "vk_reznor.h"

FN_REZNOR_COMMAND_DRAW(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDraw(command_buffer->buffer, vertex_count, instance_count, vertex_offset, instance_offset);
}

FN_REZNOR_COMMAND_DRAW_INDEXED(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDrawIndexed(command_buffer->buffer, index_count, instance_count, index_offset, vertex_offset, instance_offset);
}

FN_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDrawIndexedIndirect(command_buffer->buffer, buffer->buffer, offset, draw_count, stride);
}

FN_REZNOR_COMMAND_DRAW_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDrawIndirect(command_buffer->buffer, buffer->buffer, offset, draw_count, stride);
}

FN_REZNOR_COMMAND_DISPATCH(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDispatch(command_buffer->buffer, group_count_x, group_count_y, group_count_z);
}

FN_REZNOR_COMMAND_DISPATCH_INDIRECT(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdDispatchIndirect(command_buffer->buffer, buffer->buffer, offset);
}

attr_inline void write_rendering_attachment_info(
    VkRenderingAttachmentInfo                *write, 
    const struct reznor_rendering_attachment *param)
{
        const struct reznor_texture *texture = param->texture;
        const struct reznor_texture *resolve_texture = param->texture;

        assert_debug(texture->image && texture->image_view);
        assert_debug(resolve_texture->image && resolve_texture->image_view);

        write->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        write->pNext = NULL;
        write->imageLayout = vulkan_texture_layout_translate(param->texture_layout);
        write->imageView = texture->image_view;
        write->resolveMode = 0; /* TODO */
        write->resolveImageLayout = vulkan_texture_layout_translate(param->resolve_texture_layout);
        write->resolveImageView = resolve_texture->image_view;
        write->loadOp = vulkan_get_attachment_load_operation(param->load_op);
        write->storeOp = vulkan_get_attachment_store_operation(param->store_op);
        memcpy(&write->clearValue, &param->clear_value, sizeof(VkClearValue));
}

FN_REZNOR_COMMAND_BEGIN_RENDERING(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    struct riven *riven = device->header.reznor->interface.header.riven;

    VkRenderingAttachmentInfo *rendering_color_attachments;
    VkRenderingAttachmentInfo rendering_depth_attachment;
    VkRenderingAttachmentInfo rendering_stencil_attachment;

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderArea = {
            .offset = {
                .x = params->render_area_offset[0],
                .y = params->render_area_offset[1],
            },
            .extent = {
                .width = params->render_area_extent[0],
                .height = params->render_area_extent[1],
            },
        },
        .layerCount = 1,
    };

    /* write the attachment infos */
    if (params->color_attachments) {
        u32 color_attachment_count = params->color_attachment_count;
        rendering_color_attachments = (VkRenderingAttachmentInfo *)
            riven_alloc(riven, riven_tag_deferred, sizeof(VkRenderingAttachmentInfo) * color_attachment_count, _Alignof(VkRenderingAttachmentInfo));

        for (u32 i = 0; i < color_attachment_count; i++) {
            struct reznor_rendering_attachment *param = &params->color_attachments[i];
            VkRenderingAttachmentInfo *write = &rendering_color_attachments[i];
            write_rendering_attachment_info(write, param);
        }
        rendering_info.colorAttachmentCount = color_attachment_count;
        rendering_info.pColorAttachments = rendering_color_attachments;
    }
    if (params->depth_attachment) {
        write_rendering_attachment_info(&rendering_depth_attachment, params->depth_attachment);
        rendering_info.pDepthAttachment = &rendering_depth_attachment;
    }
    if (params->stencil_attachment) {
        write_rendering_attachment_info(&rendering_stencil_attachment, params->stencil_attachment);
        rendering_info.pStencilAttachment = &rendering_stencil_attachment;
    }
    device->vkCmdBeginRendering(command_buffer->buffer, &rendering_info);
}

FN_REZNOR_COMMAND_END_RENDERING(vulkan)
{
    struct reznor_device *device = command_buffer->device;
    device->vkCmdEndRendering(command_buffer->buffer);
}

#endif /* REZNOR_VULKAN */
