#include <amw/pelagia.h>
#include <amw/log.h>

static const PFN_rivens_encore g_native_encores[] = {
#ifdef PELAGIA_D3D12
    (PFN_rivens_encore)pelagia_d3d12_encore,
#endif
#ifdef PELAGIA_METAL
    (PFN_rivens_encore)pelagia_metal_encore,
#endif
#ifdef PELAGIA_VULKAN
    (PFN_rivens_encore)pelagia_vulkan_encore,
#endif
#ifdef PELAGIA_WEBGPU
    (PFN_rivens_encore)pelagia_webgpu_encore,
#endif
    (PFN_rivens_encore)pelagia_stub_encore,
};

const PFN_rivens_encore *pelagia_acquire_native_encores(u32 *out_count, b32 fallback)
{
    u32 count = arraysize(g_native_encores);
    *out_count = fallback ? count : max(1, count-1);
    return g_native_encores;
}

b32 pelagia_interface_validate(struct pelagia *pelagia)
{
    struct pelagia_interface *interface = (struct pelagia_interface *)pelagia;
    const char *fmt = "Pelagia '%s' is missing interface procedure - 'PFN_pelagia_%s'.";
    b32 result = true;

#define CHECK(fn) \
    if (interface->fn == NULL) { if (verbose) log_warn(fmt, interface->header.name.ptr, #fn); result = false; }

    (void)interface;
    (void)fmt;

#undef CHECK
    return result;
}

static void stub_interface_fini(struct pelagia *pelagia)
{
    (void)pelagia;
}

RIVENS_ENCORE(pelagia, stub)
{
    struct pelagia_interface *interface = (struct pelagia_interface *) 
        riven_alloc(overture->header.riven, overture->header.tag, sizeof(struct pelagia_interface), _Alignof(struct pelagia_interface));

    *interface = (struct pelagia_interface){
        .header = riven_write_interface_header(
            str_init("stub"), 
            stub_interface_fini, 
            pelagia_interface_validate),
    };
    return (struct pelagia *)interface;
}

/* XXX encore stubs, to be implemented */
#define ENCORE_STUB(backend) \
    RIVENS_ENCORE(pelagia, backend) { log_error("Pelagia encore '%s' is not yet implemented.", #backend); (void)overture; return NULL; }

#ifdef PELAGIA_D3D12
ENCORE_STUB(d3d12)
#endif
#ifdef PELAGIA_METAL
ENCORE_STUB(metal)
#endif
#ifdef PELAGIA_WEBGPU
ENCORE_STUB(webgpu)
#endif
