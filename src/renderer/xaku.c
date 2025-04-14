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

#define VALIDATE(INTERFACE, FN) \
    if (interface->INTERFACE.FN == NULL) { bedrock_log_debug(fmt, interface->header.name, interface->header.backend, #INTERFACE "_" #FN); valid = false; }

    VALIDATE(cmd, copy_buffer_to_buffer)
    VALIDATE(cmd, copy_buffer_to_texture)
    VALIDATE(cmd, copy_texture_to_buffer)
    VALIDATE(cmd, copy_texture_to_texture)
    VALIDATE(cmd, build_acceleration_structures)
    VALIDATE(cmd, clear_buffer)
    VALIDATE(cmd, clear_texture)
    VALIDATE(cmd, destroy_deferred)
    VALIDATE(cmd, push_constant)
    VALIDATE(cmd, barriers)
    VALIDATE(cmd, transitions)
    VALIDATE(cmd, set_compute_pipeline)
    VALIDATE(cmd, set_raster_pipeline)
    VALIDATE(cmd, set_ray_tracing_pipeline)
    VALIDATE(cmd, set_viewport)
    VALIDATE(cmd, set_scissor)
    VALIDATE(cmd, set_depth_bias)
    VALIDATE(cmd, set_index_buffer)
    VALIDATE(cmd, begin_rendering)
    VALIDATE(cmd, end_rendering)
    VALIDATE(cmd, dispatch)
    VALIDATE(cmd, dispatch_indirect)
    VALIDATE(cmd, draw)
    VALIDATE(cmd, draw_indexed)
    VALIDATE(cmd, draw_indirect)
    VALIDATE(cmd, draw_indirect_count)
    VALIDATE(cmd, draw_mesh_tasks)
    VALIDATE(cmd, draw_mesh_tasks_indirect)
    VALIDATE(cmd, draw_mesh_tasks_indirect_count)
    VALIDATE(cmd, trace_rays)
    VALIDATE(cmd, trace_rays_indirect)
    VALIDATE(cmd, write_timestamps)
    VALIDATE(cmd, reset_timestamps)
    VALIDATE(cmd, begin_label)
    VALIDATE(cmd, end_label)
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

FN_RIVEN_ENCORE_STUB(xaku, null);
