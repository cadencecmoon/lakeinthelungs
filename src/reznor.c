#include <amw/log.h>
#include <amw/reznor.h>

RIVEN_ENCORE(reznor, native)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#ifdef REZNOR_D3D12
        (PFN_riven_work)reznor_encore_d3d12,
#endif
#ifdef REZNOR_METAL
        (PFN_riven_work)reznor_encore_metal,
#endif
#ifdef REZNOR_VULKAN
        (PFN_riven_work)reznor_encore_vulkan,
#endif
#ifdef REZNOR_WEBGPU
        (PFN_riven_work)reznor_encore_webgpu,
#endif
#ifndef REZNOR_DISABLE_NULL_FALLBACK
        (PFN_riven_work)reznor_encore_null,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i]((riven_argument_t)encore);

        if (*encore->header.interface == NULL) 
            continue;

        const struct reznor_interface *interface = *encore->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_reznor_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_debug(fmt, interface->header.name.ptr, #fn); valid = false; }
        VALIDATE(device_query)
        VALIDATE(device_create)
        VALIDATE(device_destroy)
        VALIDATE(device_memory_assembly)
        VALIDATE(buffer_assembly)
        VALIDATE(texture_assembly)
        VALIDATE(sampler_assembly)
        VALIDATE(descriptor_set_layout_assembly)
        VALIDATE(descriptor_set_assembly)
        VALIDATE(pipeline_layout_assembly)
        VALIDATE(graphics_pipeline_assembly)
        VALIDATE(compute_pipeline_assembly)
        VALIDATE(raytracing_pipeline_assembly)
        VALIDATE(shader_binding_table_assembly)
        VALIDATE(bottom_level_assembly)
        VALIDATE(top_level_assembly)
        VALIDATE(query_pool_assembly)
        VALIDATE(swapchain_assembly)
        VALIDATE(swapchain_try_recreate)
        VALIDATE(swapchain_next_image)
        VALIDATE(disassembly)
        VALIDATE(frame_begin)
        VALIDATE(frame_submit)
        VALIDATE(command_draw)
        VALIDATE(command_draw_indexed)
        VALIDATE(command_draw_indexed_indirect)
        VALIDATE(command_draw_indirect)
        VALIDATE(command_dispatch)
        VALIDATE(command_dispatch_indirect)
        VALIDATE(command_copy_buffer)
        VALIDATE(command_copy_texture)
        VALIDATE(command_begin_render_pass)
        VALIDATE(command_end_render_pass)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((riven_argument_t)interface);
        *encore->header.interface = NULL;
    }
}

RIVEN_ENCORE(reznor, null)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    struct reznor_interface *interface = (struct reznor_interface *) 
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct reznor_interface), _Alignof(struct reznor_interface));

    /* write the interface */
    interface->header.name = str_init("reznor_null");
    interface->header.riven = encore->header.riven;
    interface->header.tag = encore->header.tag;
    interface->header.fini = (PFN_riven_work)riven_work_nop;

    *encore->header.interface = (riven_argument_t)(struct reznor *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef REZNOR_D3D12
RIVEN_ENCORE_STUB(reznor, d3d12)
#endif
#ifdef REZNOR_METAL
RIVEN_ENCORE_STUB(reznor, metal)
#endif
#ifdef REZNOR_WEBGPU
RIVEN_ENCORE_STUB(reznor, webgpu)
#endif

AMWAPI void AMWCALL reznor_devices_assembly(struct reznor_devices_assembly_work *restrict work)
{
    (void)work;
}
