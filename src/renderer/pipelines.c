/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/pelagia.h>

/* The implementation for a specific render pass will know what index to pull from 
 * pipeline state. Pipelines can be created concurrently, and all the render targets
 * or uniform buffers, and other data needed to assemble the pipelines, is not mutable
 * here. No external synchronization is needed, so we will *sure* exploit this here :) */
struct assemble_pipeline_work {
    struct vulkan_device   *device; 
    b32                     dissasemble;
    s32                     out_result;
};

/* Shared dissassemble code. */
AMW_INLINE void dissasemble(const struct vulkan_device *device, struct vulkan_pipeline *pipeline)
{
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
 *
static void assemble_pipeline_render_pass(struct assemble_pipeline_work *work)
{
    const struct vulkan_device *device   = work->device; 
    struct vulkan_pipeline     *pipeline = &work->device->pipelines[render_pass_type];

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

    work->out_result = result_error_undefined;
}
 */

static void assemble_pipeline_visibility_pass(struct assemble_pipeline_work *work)
{
    const struct vulkan_device *device   = work->device; 
    struct vulkan_pipeline     *pipeline = &work->device->pipelines[render_pass_type_visibility];

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_lighting_pass(struct assemble_pipeline_work *work)
{
    const struct vulkan_device *device   = work->device; 
    struct vulkan_pipeline     *pipeline = &work->device->pipelines[render_pass_type_lighting];

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_forward_pass(struct assemble_pipeline_work *work)
{
    const struct vulkan_device *device   = work->device; 
    struct vulkan_pipeline     *pipeline = &work->device->pipelines[render_pass_type_forward];

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

    work->out_result = result_error_undefined; /* TODO */
}

static void assemble_pipeline_interface_pass(struct assemble_pipeline_work *work)
{
    const struct vulkan_device *device   = work->device; 
    struct vulkan_pipeline     *pipeline = &work->device->pipelines[render_pass_type_interface];

    dissasemble(device, pipeline);
    if (work->dissasemble) return;

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
    AMW_INLINE void assemble_pipeline_##proc##_pass_tear__( \
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
    struct pelagia               *pelagia = work->pelagia;
    struct riven                 *riven   = work->riven;
    struct rivens_tear            tears[render_pass_type_count];
    struct assemble_pipeline_work pipeline_work[render_pass_type_count];

    /* XXX i could control if a specific pipeline should be touched or not
     * by passing a flags value in the work, and retrieving the state of those 
     * bits with (flags >> j) check, where j is a render_pass_type. Another
     * optimization would be to ignore assigning a tear, depending on if the 
     * bit for this specific render pass pipeline is set or not. */
#define ASSEMBLE_PIPELINE_TEAR(proc) \
    assemble_pipeline_##proc##_pass_tear__(&pipeline_work[render_pass_type_##proc], &tears[render_pass_type_##proc]);
    ASSEMBLE_PIPELINE_TEAR(visibility)
    ASSEMBLE_PIPELINE_TEAR(lighting)
    ASSEMBLE_PIPELINE_TEAR(forward)
    ASSEMBLE_PIPELINE_TEAR(interface)
#undef ASSEMBLE_PIPELINE_TEAR

    /* A pipeline state is device-specific. We will handle one device at a time,
     * and assemble the pipelines for all available devices. */
    for (u32 i = 0; i < pelagia->device_count; i++) {
        struct vulkan_device *device = &pelagia->devices[i];

        assert_debug(device->logical != VK_NULL_HANDLE);

        /* prepare the arguments */
        for (u32 j = 0; j < render_pass_type_count; j++) {
            pipeline_work[j].device      = device;
            pipeline_work[j].dissasemble = work->dissasemble;
            pipeline_work[j].out_result  = result_success;
        }
        /* execute the tears */
        riven_split_and_unchain(riven, tears, render_pass_type_count);

        /* check results */
        for (u32 j = 0; j < render_pass_type_count; j++) {
            if (pipeline_work[j].out_result != result_success) {
                log_error("Tear %s failed to resolve a conflict and returned %d. The pipeline state of the %s render pass is invalid.", tears[j].name, pipeline_work[j].out_result, render_pass_name_from_idx(j));
                work->out_result = pipeline_work[j].out_result; /* return the last error code */
            }
        }
    }
}
