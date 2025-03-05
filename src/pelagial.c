#include <amw/pelagial.h>

struct pelagic_ocean *pelagial_init(
    const PFN_pelagial_entry_point         *entries,
    u32                                     entry_count,
    const struct pelagic_ocean_create_info *create_info,
    b32                                     verbose)
{
    struct pelagic_ocean *pelagial = NULL;
    if (entry_count == 0) {
        if (verbose)
            log_error("Pelagial: no entry points were given.");
        return NULL;
    }

    for (u32 o = 0; o < entry_count; o++) {
        struct pelagic_ocean_interface interface = {0};
        s32 res = entries[o](&interface, create_info, verbose);

        if (res == result_reiterate) {
            pelagial = (struct pelagic_ocean *)interface.internal;
            if (verbose)
                log_info("Pelagial: sharing an existing rendering backend of id %u '%s'.", pelagial->interface.id, pelagial->interface.name.ptr);
            return pelagial;
        } else if (res != result_success) {
            continue;
        }

        const char *err = "Pelagial: id %u '%s' is missing interface procedure - 'PFN_pelagial_%s'.";
#define INTERFACE_CHECK(fn) \
        if (interface.fn == NULL) { log_warn(err, interface.id, interface.name.ptr, #fn); res = result_error; }

        INTERFACE_CHECK(renderer_init);
        INTERFACE_CHECK(renderer_fini);

#undef INTERFACE_CHECK
        if (res != result_success) {
            if (interface.renderer_fini)
                interface.renderer_fini(interface.internal);
            continue;
        }

        if (pelagial == NULL) {
            pelagial = (struct pelagic_ocean *)riven_alloc(create_info->riven, create_info->tag, sizeof(struct pelagic_ocean), _Alignof(struct pelagic_ocean));
            zerop(pelagial);
        }

        pelagial->interface = interface;
        pelagial->riven = create_info->riven;
        pelagial->tag = create_info->tag;

        res = pelagial->interface.renderer_init(pelagial, create_info);
        if (res != result_success) {
            pelagial_fini(pelagial);
            continue;
        }
        assert_debug(atomic_load_explicit(&pelagial->flags, memory_order_relaxed) & pelagic_flag_renderer_is_valid);

        if (verbose)
            log_info("Pelagial: initialized a rendering backend of id %d '%s'.", interface.id, interface.name.ptr);
        return pelagial;
    }
    if (verbose)
        log_error("Pelagial: tried %u entry points, none of them resulted in a valid rendering backend.", entry_count);
    return NULL;
}

void pelagial_fini(struct pelagic_ocean *pelagial)
{
    if (pelagial->interface.renderer_fini)
        pelagial->interface.renderer_fini(pelagial->interface.internal);
    zerop(pelagial);
}

void pelagial_touch_swapchain(struct pelagic_work_touch_swapchain *restrict work)
{
    (void)work;
    return;
}

/** Backend entry point definitions for builds where such a backend is not supported. */
#define ENTRY_POINT_STUB(TYPE)                                                               \
    PELAGIAL_ENTRY_POINT(TYPE) {                                                             \
        (void)create_info;                                                                   \
        *interface = (struct pelagic_ocean_interface){                                       \
            .id = pelagic_backend_##TYPE,                                                    \
            .name = string_init(#TYPE),                                                      \
        };                                                                                   \
        if (verbose)                                                                         \
            log_error("Pelagial: rendering backend %u '%s' is not supported in this build.", \
                interface->id, interface->name.ptr);                                         \
        return result_error;                                                                 \
    }

/* TODO to be implemented */
ENTRY_POINT_STUB(d3d12)
ENTRY_POINT_STUB(metal)
ENTRY_POINT_STUB(webgpu)

#ifndef PELAGIAL_D3D12
    /* TODO */
#endif
#ifndef PELAGIAL_METAL
    /* TODO */
#endif
#ifndef PELAGIAL_VULKAN
    ENTRY_POINT_STUB(vulkan)
#endif
#ifndef PELAGIAL_WEBGPU
    /* TODO */
#endif
#undef ENTRY_POINT_STUB

s32 pelagial_entry_point(_PELAGIAL_ENTRY_POINT_ARGS)
{
    (void)interface; (void)create_info; (void)verbose;

    s32 res = result_error;
#define ENTRY_POINT_CHECK(name)                                           \
    res = pelagial_##name##_entry_point(interface, create_info, verbose); \
    if (res == result_success || res == result_reiterate) return res;

#ifdef PELAGIAL_D3D12
    ENTRY_POINT_CHECK(d3d12)
#endif
#ifdef PELAGIAL_METAL
    ENTRY_POINT_CHECK(metal)
#endif
#ifdef PELAGIAL_VULKAN
    ENTRY_POINT_CHECK(vulkan)
#endif
#ifdef PELAGIAL_WEBGPU
    ENTRY_POINT_CHECK(webgpu)
#endif

#undef ENTRY_POINT_CHECK
    *interface = (struct pelagic_ocean_interface){
        .id = pelagic_backend_invalid,
        .name = string_init("auto"),
    };
    return res;
}

s32 pelagial_jester_renderer_init(
    struct pelagic_ocean                   *restrict pelagial,
    const struct pelagic_ocean_create_info *restrict create_info)
{
    (void)pelagial; (void)create_info;
    atomic_fetch_or_explicit(&pelagial->flags, pelagic_flag_renderer_is_valid, memory_order_release);
    return result_success;
}

void pelagial_jester_renderer_fini(void *restrict internal)
{
    (void)internal;
}

PELAGIAL_ENTRY_POINT(jester)
{
    (void)create_info; (void)verbose;

    *interface = (struct pelagic_ocean_interface){
        .id = pelagic_backend_jester,
        .name = string_init("jester"),
        .internal = NULL,
        .renderer_init = pelagial_jester_renderer_init,
        .renderer_fini = pelagial_jester_renderer_fini,
    };
    return result_success;
}
