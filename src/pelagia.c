#include <amw/pelagia.h>
#include <amw/log.h>

RIVEN_ENCORE(pelagia, native)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#ifdef PELAGIA_D3D12
        (PFN_riven_work)pelagia_encore_d3d12,
#endif
#ifdef PELAGIA_METAL
        (PFN_riven_work)pelagia_encore_metal,
#endif
#ifdef PELAGIA_VULKAN
        (PFN_riven_work)pelagia_encore_vulkan,
#endif
#ifdef PELAGIA_WEBGPU
        (PFN_riven_work)pelagia_encore_webgpu,
#endif
#ifndef PELAGIA_DISABLE_STUB_FALLBACK
        (PFN_riven_work)pelagia_encore_stub,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i]((riven_argument_t)create_info);

        if (*create_info->header.interface == NULL) 
            continue;

        const struct pelagia_interface *interface = *create_info->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_pelagia_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_warn(fmt, interface->header.name.ptr, #fn); valid = false; }
        VALIDATE(query_physical_devices)
        VALIDATE(create_device)
        VALIDATE(destroy_device)
        VALIDATE(create_buffers)
        VALIDATE(create_textures)
        VALIDATE(create_samplers)
        VALIDATE(create_shaders)
        VALIDATE(create_pipeline_layouts)
        VALIDATE(create_graphics_pipelines)
        VALIDATE(create_compute_pipelines)
        VALIDATE(create_raytracing_pipelines)
        VALIDATE(create_shader_binding_tables)
        VALIDATE(create_descriptor_sets)
        VALIDATE(create_query_pools)
        VALIDATE(create_swapchains)
        VALIDATE(create_bottom_levels)
        VALIDATE(create_top_levels)
        VALIDATE(destroy_resources)
        VALIDATE(recreate_swapchain)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((riven_argument_t)interface);
        *create_info->header.interface = NULL;
    }
}

RIVEN_ENCORE(pelagia, stub)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    struct riven *riven = create_info->header.riven;
    riven_tag_t tag = create_info->header.tag;

    struct pelagia_interface *interface = (struct pelagia_interface *) 
        riven_alloc(riven, tag, sizeof(struct pelagia_interface), _Alignof(struct pelagia_interface));

    *interface = (struct pelagia_interface){
        .header = {
            .name = str_init("pelagia_stub"),
            .riven = riven,
            .tag = tag,
            .fini = riven_work_nop,
        },
    };
    *create_info->header.interface = (riven_argument_t)(struct pelagia *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef PELAGIA_D3D12
RIVEN_ENCORE_STUB(pelagia, d3d12)
#endif
#ifdef PELAGIA_METAL
RIVEN_ENCORE_STUB(pelagia, metal)
#endif
#ifdef PELAGIA_WEBGPU
RIVEN_ENCORE_STUB(pelagia, webgpu)
#endif
