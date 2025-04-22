#include <amwe/xaku.h>

static const PFN_riven_encore native_encores[] = {
#ifdef XAKU_D3D12
    (PFN_riven_encore)xaku_encore_d3d12,
#endif /* XAKU_D3D12 */
#ifdef XAKU_METAL
    (PFN_riven_encore)xaku_encore_metal,
#endif /* XAKU_METAL */
#ifdef XAKU_WEBGPU
    (PFN_riven_encore)xaku_encore_webgpu,
#endif /* XAKU_WEBGPU */
#ifdef XAKU_VULKAN
    (PFN_riven_encore)xaku_encore_vulkan,
#endif /* XAKU_VULKAN */
    (PFN_riven_encore)xaku_encore_null,
};

const PFN_riven_encore *xaku_native_encores(bool null_fallback, u32 *out_encore_count)
{
    u32 encore_count = lake_arraysize(native_encores);
    if (encore_count > 1 && !null_fallback)
        encore_count--;

    *out_encore_count = encore_count;
    return native_encores;
}

extern FN_RIVEN_INTERFACE_VALIDATION(xaku)
{
    const char *fmt = "'%s: %s' is missing an interface procedure - 'PFN_xaku_%s'.";
    bool valid = true;

#define VALIDATE(FN) \
    if (interface->FN == NULL) { bedrock_log_debug(fmt, interface->header.name, interface->header.backend, #FN); valid = false; }

    VALIDATE(list_devices_properties)
    VALIDATE(device_assembly)
    VALIDATE(device_disassembly)
    VALIDATE(device_queue_count)
    VALIDATE(device_queue_wait_idle)
    VALIDATE(device_wait_idle)
    VALIDATE(device_submit)
    VALIDATE(device_present)
    VALIDATE(memory_buffer_requirements)
    VALIDATE(memory_texture_requirements)
    VALIDATE(memory_assembly)
    VALIDATE(memory_disassembly)
    VALIDATE(query_pool_assembly)
    VALIDATE(query_pool_disassembly)
    VALIDATE(swapchain_assembly)
    VALIDATE(swapchain_disassembly)
    VALIDATE(compute_pipeline_assembly)
    VALIDATE(compute_pipeline_disassembly)
    VALIDATE(raster_pipeline_assembly)
    VALIDATE(raster_pipeline_disassembly)
    VALIDATE(ray_tracing_pipeline_assembly)
    VALIDATE(ray_tracing_pipeline_disassembly)
    VALIDATE(command_recorder_assembly)
    VALIDATE(command_recorder_disassembly)
    VALIDATE(deferred_resource_disassembly)
    VALIDATE(create_buffer)
    VALIDATE(create_buffer_from_memory)
    VALIDATE(create_texture)
    VALIDATE(create_texture_from_memory)
    VALIDATE(create_texture_view)
    VALIDATE(create_sampler)
    VALIDATE(create_tlas)
    VALIDATE(create_blas)
    VALIDATE(create_tlas_from_buffer)
    VALIDATE(create_blas_from_buffer)
    VALIDATE(get_buffer_assembly)
    VALIDATE(get_texture_assembly)
    VALIDATE(get_texture_view_assembly)
    VALIDATE(get_sampler_assembly)
    VALIDATE(get_tlas_assembly)
    VALIDATE(get_tlas_build_sizes)
    VALIDATE(get_blas_assembly)
    VALIDATE(get_blas_build_sizes)
    VALIDATE(destroy_buffer)
    VALIDATE(destroy_texture)
    VALIDATE(destroy_texture_view)
    VALIDATE(destroy_sampler)
    VALIDATE(destroy_tlas)
    VALIDATE(destroy_blas)
    VALIDATE(cmd_copy_buffer_to_buffer)
    VALIDATE(cmd_copy_buffer_to_texture)
    VALIDATE(cmd_copy_texture_to_buffer)
    VALIDATE(cmd_copy_texture_to_texture)
    VALIDATE(cmd_clear_buffer)
    VALIDATE(cmd_clear_texture)
    VALIDATE(cmd_build_acceleration_structures)
    VALIDATE(cmd_destroy_buffer_deferred)
    VALIDATE(cmd_destroy_texture_deferred)
    VALIDATE(cmd_destroy_texture_view_deferred)
    VALIDATE(cmd_destroy_sampler_deferred)
    VALIDATE(cmd_destroy_tlas_deferred)
    VALIDATE(cmd_destroy_blas_deferred)
    VALIDATE(cmd_push_constant)
    VALIDATE(cmd_set_compute_pipeline)
    VALIDATE(cmd_set_raster_pipeline)
    VALIDATE(cmd_set_ray_tracing_pipeline)
    VALIDATE(cmd_set_viewport)
    VALIDATE(cmd_set_scissor)
    VALIDATE(cmd_set_depth_bias)
    VALIDATE(cmd_set_index_buffer)
    VALIDATE(cmd_begin_rendering)
    VALIDATE(cmd_end_rendering)
    VALIDATE(cmd_dispatch)
    VALIDATE(cmd_dispatch_indirect)
    VALIDATE(cmd_draw)
    VALIDATE(cmd_draw_indexed)
    VALIDATE(cmd_draw_indirect)
    VALIDATE(cmd_draw_indirect_count)
    VALIDATE(cmd_mesh_tasks)
    VALIDATE(cmd_mesh_tasks_indirect)
    VALIDATE(cmd_mesh_tasks_indirect_count)
    VALIDATE(cmd_trace_rays)
    VALIDATE(cmd_trace_rays_indirect)
    VALIDATE(cmd_write_timestamps)
    VALIDATE(cmd_reset_timestamps)
    VALIDATE(cmd_begin_label)
    VALIDATE(cmd_end_label)
    VALIDATE(compile_command_list)
#undef VALIDATE

    return valid;
}

#ifdef XAKU_D3D12
FN_RIVEN_ENCORE_STUB(xaku, d3d12)
#endif /* XAKU_D3D12 */
#ifdef XAKU_METAL
FN_RIVEN_ENCORE_STUB(xaku, metal)
#endif /* XAKU_METAL */
#ifdef XAKU_WEBGPU
FN_RIVEN_ENCORE_STUB(xaku, webgpu)
#endif /* XAKU_WEBGPU */

enum xaku_result xaku_renderer_init(
    const struct xaku_renderer_assembly *assembly,
    struct xaku_renderer                *out_renderer)
{
    out_renderer->xaku = assembly->xaku;
    out_renderer->strategy = assembly->strategy;

    riven_inc_refcnt(&out_renderer->xaku.header->refcnt);
    return xaku_result_success;
}

u32 xaku_renderer_fini(struct xaku_renderer *renderer, struct riven_work *out_zero_refcnt)
{
    if (!renderer->xaku.data)
        return 0;

    u32 prev = riven_dec_refcnt(&renderer->xaku.header->refcnt);
    if (prev == 1) {
        out_zero_refcnt->procedure = renderer->xaku.header->zero_ref_callback;
        out_zero_refcnt->userdata = renderer->xaku.data;
        out_zero_refcnt->name = "xaku_fini:zero_refcnt";
        bedrock_log_verbose("xaku zero ref trace");
    }
    return prev;
}
