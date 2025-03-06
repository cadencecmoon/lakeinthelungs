#include <amw/silvera.h>
#include <amw/log.h>

static const PFN_rivens_encore g_native_encores[] = {
#ifdef SILVERA_D3D12
    (PFN_rivens_encore)silvera_d3d12_encore,
#endif
#ifdef SILVERA_METAL
    (PFN_rivens_encore)silvera_metal_encore,
#endif
#ifdef SILVERA_VULKAN
    (PFN_rivens_encore)silvera_vulkan_encore,
#endif
#ifdef SILVERA_WEBGPU
    (PFN_rivens_encore)silvera_webgpu_encore,
#endif
    (PFN_rivens_encore)silvera_stub_encore,
};

const PFN_rivens_encore *silvera_acquire_native_encores(u32 *out_count, b32 fallback)
{
    u32 count = arraysize(g_native_encores);
    *out_count = fallback ? count : max(1, count-1);
    return g_native_encores;
}

static b32 _silvera_interface_validate(struct silvera *silvera)
{
    struct silvera_interface *interface = (struct silvera_interface *)silvera;
    const char *fmt = "Silvera '%s' is missing interface procedure - 'PFN_silvera_%s'.";
    b32 result = true;

#define CHECK(fn) \
    if (interface->fn == NULL) { if (verbose) log_warn(fmt, interface->header.name.ptr, #fn); result = false; }

    (void)interface;
    (void)fmt;

#undef CHECK
    return result;
}

static void stub_interface_fini(struct silvera *silvera)
{
    (void)silvera;
}

RIVENS_ENCORE(silvera, stub)
{
    struct silvera_interface *interface = (struct silvera_interface *) 
        riven_alloc(overture->header.riven, overture->header.tag, sizeof(struct silvera_interface), _Alignof(struct silvera_interface));

    *interface = (struct silvera_interface){
        .header = riven_write_interface_header(
            UINT32_MAX, str_init("stub"), 
            stub_interface_fini, 
            _silvera_interface_validate),
    };
    return (struct silvera *)interface;
}
