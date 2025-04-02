#include <amw/bedrock/log.h>
#include <amw/reznor.h>

static const char *attr_const string_from_resource_type(enum reznor_resource_type type)
{
    switch (type) {
#define RESTYPE(name) \
        case reznor_resource_type_##name: return #name;
        RESTYPE(device_memory)
        RESTYPE(buffer)
        RESTYPE(texture)
        RESTYPE(sampler)
        RESTYPE(descriptor_set_layout)
        RESTYPE(descriptor_set)
        RESTYPE(pipeline_layout)
        RESTYPE(graphics_pipeline)
        RESTYPE(compute_pipeline)
        RESTYPE(raytracing_pipeline)
        RESTYPE(shader_binding_table)
        RESTYPE(bottom_level)
        RESTYPE(top_level)
        RESTYPE(query_pool)
        RESTYPE(swapchain)
        default: return "unknown";
#undef RESTYPE
    }
}

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
        encores[i](encore);

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
        VALIDATE(frame_begin)
        VALIDATE(frame_next_images)
        VALIDATE(frame_submit)
        for (u32 i = 0; i < reznor_resource_type_count; i++) {
            if (interface->assembly_table[i] == NULL || interface->disassembly_table[i] == NULL) {
                const char *resource = string_from_resource_type(i);

                log_debug("'%s' is missing interface assembly/disassembly dispatch table procedure: %u of '%s'.", 
                    interface->header.name.ptr, i, resource);
                valid = false;
            }
        }
        VALIDATE(try_recreate_swapchain)
        VALIDATE(command_draw)
        VALIDATE(command_draw_indexed)
        VALIDATE(command_draw_indexed_indirect)
        VALIDATE(command_draw_indirect)
        VALIDATE(command_dispatch)
        VALIDATE(command_dispatch_indirect)
        VALIDATE(command_copy_buffer)
        VALIDATE(command_copy_texture)
        VALIDATE(command_begin_rendering)
        VALIDATE(command_end_rendering)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((void *)interface);
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
    interface->thread_count = encore->thread_count;
    interface->debug_utils = encore->debug_utils;

    *encore->header.interface = (void *)(struct reznor *)interface;
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
