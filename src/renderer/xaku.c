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

    (void)interface;
    (void)fmt;
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
