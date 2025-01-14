/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/pelagia.h>

AMWAPI s32 vulkan_create_descriptor_sets(
    struct vulkan_pipeline                     *pipeline,
    struct vulkan_device                       *device,
    const struct vulkan_descriptor_set_request *descriptor_set_request,
    u32                                         descriptor_set_count)
{
    (void)pipeline;
    (void)device;
    (void)descriptor_set_request;
    (void)descriptor_set_count;
    return result_error_undefined;
}

AMWAPI void vulkan_complete_descriptor_set_writes(
    VkWriteDescriptorSet                       *writes,
    const struct vulkan_descriptor_set_request *descriptor_set_request,
    u32                                         write_count)
{
    (void)writes;
    (void)descriptor_set_request;
    (void)write_count;
}

/* The implementation for a specific render pass will know what index to pull from 
 * pipeline state. Pipelines can be created concurrently, and all the render targets
 * or uniform buffers, and other data needed to assemble the pipelines, is not mutable
 * here. No external synchronization is needed, so we will *sure* exploit this here :) */
struct assemble_pipeline_work {
    struct vulkan_backend  *vk;
    struct vulkan_device   *device; 
    VkPipelineCache         cache;
    b32                     dissasemble;
    s32                     out_result;
};

/* Shared cleanup code. */
static void dissasemble(
    const struct vulkan_backend *vk, 
    const struct vulkan_device  *device, 
    struct vulkan_pipeline      *pipeline)
{
    (void)vk; // TODO &vk->allocator
    if (pipeline->pipeline)
        device->api.vkDestroyPipeline(device->logical, pipeline->pipeline, NULL); /* TODO allocation callbacks */
    if (pipeline->descriptor_pool)
        device->api.vkDestroyDescriptorPool(device->logical, pipeline->descriptor_pool, NULL); /* TODO allocation callbacks */
    if (pipeline->pipeline_layout)
        device->api.vkDestroyPipelineLayout(device->logical, pipeline->pipeline_layout, NULL); /* TODO allocation callbacks */
    if (pipeline->descriptor_set_layout)
        device->api.vkDestroyDescriptorSetLayout(device->logical, pipeline->descriptor_set_layout, NULL); /* TODO allocation callbacks */
    iazerop(pipeline);
}

/* XXX copy-paste template for an assemble_pipeline_render_pass procedure:

static void assemble_pipeline_render_pass(struct assemble_pipeline_work *work)
{
    if (!work) return;

    const struct vulkan_backend *vk       = work->vk; 
    const struct vulkan_device  *device   = work->device; 
    struct vulkan_pipeline      *pipeline = &work->device->pipelines[render_pass_type];
    VkPipelineCache              cache    = work->cache;

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

    // DO THE WORK

    work->out_result = result_error_undefined;
}
 */

static void assemble_pipeline_visibility_pass(struct assemble_pipeline_work *work)
{
    if (!work) return;

    const struct vulkan_backend *vk       = work->vk; 
    const struct vulkan_device  *device   = work->device; 
    struct vulkan_pipeline      *pipeline = &work->device->pipelines[render_pass_type_visibility];
    VkPipelineCache              cache    = work->cache;

    dissasemble(vk, device, pipeline);
    if (work->dissasemble) return;

    (void)cache;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_lighting_pass(struct assemble_pipeline_work *work)
{
    if (!work) return;

    const struct vulkan_backend *vk       = work->vk; 
    const struct vulkan_device  *device   = work->device; 
    struct vulkan_pipeline      *pipeline = &work->device->pipelines[render_pass_type_lighting];
    VkPipelineCache              cache    = work->cache;

    dissasemble(vk, device, pipeline);
    if (work->dissasemble) return;

    (void)cache;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_forward_pass(struct assemble_pipeline_work *work)
{
    if (!work) return;

    const struct vulkan_backend *vk       = work->vk; 
    const struct vulkan_device  *device   = work->device; 
    struct vulkan_pipeline      *pipeline = &work->device->pipelines[render_pass_type_forward];
    VkPipelineCache              cache    = work->cache;

    dissasemble(vk, device, pipeline);
    if (work->dissasemble) return;

    (void)cache;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_interface_pass(struct assemble_pipeline_work *work)
{
    if (!work) return;

    const struct vulkan_backend *vk       = work->vk; 
    const struct vulkan_device  *device   = work->device; 
    struct vulkan_pipeline      *pipeline = &work->device->pipelines[render_pass_type_interface];
    VkPipelineCache              cache    = work->cache;

    dissasemble(vk, device, pipeline);
    if (work->dissasemble) return;

    (void)cache;

    work->out_result = result_error_undefined; /* TODO */
}

static const char *render_pass_name_from_idx(enum render_pass_type idx)
{
    switch (idx) {
#define RENDER_PASS_NAME(proc) case render_pass_type_##proc: return #proc;
        RENDER_PASS_NAME(visibility)
        RENDER_PASS_NAME(lighting)
        RENDER_PASS_NAME(forward)
        RENDER_PASS_NAME(interface)
#undef RENDER_PASS_NAME
        case render_pass_type_count:
        default:
            break;
    }
    return "unknown";
}

#define ASSEMBLE_PIPELINE_TEAR_PROCEDURE(proc) \
    static AMW_INLINE void assemble_pipeline_##proc##_pass_tear__( \
        struct assemble_pipeline_work *work, \
        struct rivens_tear *tear) \
    { \
        tear->procedure = (PFN_riven_work)assemble_pipeline_##proc##_pass; \
        tear->argument = work; \
        tear->name = "pelagia:assemble_pipeline:" #proc "_pass"; \
    }
ASSEMBLE_PIPELINE_TEAR_PROCEDURE(visibility)
ASSEMBLE_PIPELINE_TEAR_PROCEDURE(lighting)
ASSEMBLE_PIPELINE_TEAR_PROCEDURE(forward)
ASSEMBLE_PIPELINE_TEAR_PROCEDURE(interface)
#undef ASSEMBLE_PIPELINE_TEAR_PROCEDURE

AMWAPI void pelagia_assemble_render_pass_pipelines(struct pelagia_assemble_render_pass_pipelines_work *work)
{
    if (!work || work->render_pass_type_mask == 0llu) {
        return;
    } else if (!work->pelagia->devices || work->device_idx >= work->pelagia->device_count) {
        work->out_result = result_error_invalid_engine_context;
        return;
    }

    struct vulkan_backend        *vk      = &work->pelagia->vk;
    struct vulkan_device         *device  = &work->pelagia->devices[work->device_idx];
    struct riven                 *riven   = work->riven;

    struct rivens_tear            tears[render_pass_type_count];
    struct assemble_pipeline_work pipeline_work[render_pass_type_count];

    /* We want to filter out the work in a case where only explicit pipelines are to be assembled.
     * If it was not requested, the work argument in the tear will be NULL and return immediately. */
#define ASSEMBLE_PIPELINE_TEAR(proc) \
    assemble_pipeline_##proc##_pass_tear__((work->render_pass_type_mask & (1llu << render_pass_type_##proc)) != 0 ? &pipeline_work[render_pass_type_##proc] : NULL, &tears[render_pass_type_##proc]);
    ASSEMBLE_PIPELINE_TEAR(visibility)
    ASSEMBLE_PIPELINE_TEAR(lighting)
    ASSEMBLE_PIPELINE_TEAR(forward)
    ASSEMBLE_PIPELINE_TEAR(interface)
#undef ASSEMBLE_PIPELINE_TEAR

    /* TODO implement a pipeline cache */
    VkPipelineCache cache = VK_NULL_HANDLE;

    (void)cache;

    /* prepare the arguments */
    for (u32 j = 0; j < render_pass_type_count; j++) {
        pipeline_work[j].vk          = vk;
        pipeline_work[j].device      = device;
        pipeline_work[j].dissasemble = work->dissasemble;
        pipeline_work[j].out_result  = result_success;
    }
    /* assemble the pipelines */
    riven_split_and_unchain(riven, tears, render_pass_type_count);

    /* check results */
    for (u32 j = 0; j < render_pass_type_count; j++) {
        if (pipeline_work[j].out_result != result_success) {
            log_error("Tear '%s' for device '%u' failed to resolve a conflict and returned %d. The pipeline state of the %s render pass is invalid.", tears[j].name, work->device_idx, pipeline_work[j].out_result, render_pass_name_from_idx(j));
            work->out_result = pipeline_work[j].out_result; /* return the last error code */
        }
    }
}
