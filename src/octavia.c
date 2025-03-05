#include <amw/octavia.h>
#include <amw/log.h>

struct octavarium *octavia_init(
    PFN_octavia_entry_point       *entries,
    u32                            entry_count,
    struct octavarium_create_info *create_info,
    b32                            verbose)
{
    struct octavarium *octavia = NULL;
    if (entry_count == 0) {
        if (verbose)
            log_error("Octavia: no entry points were given.");
        return NULL;
    }

    for (u32 o = 0; o < entry_count; o++) {
        struct octavarium_interface interface = {0};
        s32 res = entries[o](&interface, create_info, verbose);

        if (res == result_reiterate) {
            octavia = (struct octavarium *)interface.internal;
            if (verbose)
                log_info("Octavia: sharing an existing audio backend of id %u '%s'.", octavia->interface.id, octavia->interface.name.ptr);
            return octavia;
        } else if (res != result_success) {
            continue;
        }

        const char *err = "Octavia: id %u '%s' is missing interface procedure - 'PFN_octavia_%s'.";
#define INTERFACE_CHECK(fn) \
        if (interface.fn == NULL) { log_warn(err, interface.id, interface.name.ptr, #fn); res = result_error; }

        INTERFACE_CHECK(audio_init);
        INTERFACE_CHECK(audio_fini);

#undef INTERFACE_CHECK
        if (res != result_success) {
            if (interface.audio_fini)
                interface.audio_fini(interface.internal);
            continue;
        }

        if (octavia == NULL) {
            octavia = (struct octavarium *)riven_alloc(create_info->riven, create_info->tag, sizeof(struct octavarium), _Alignof(struct octavarium));
            zerop(octavia);
        }

        octavia->interface = interface;
        octavia->riven = create_info->riven;
        octavia->tag = create_info->tag;

        res = octavia->interface.audio_init(octavia, create_info);
        if (res != result_success) {
            octavia_fini(octavia);
            continue;
        }
        if (verbose)
            log_info("Octavia: initialized an audio backend of id %d '%s'.", interface.id, interface.name.ptr);
        return octavia;
    }
    if (verbose)
        log_error("Octavia: tried %u entry points, none of them resulted in a valid audio backend.", entry_count);
    return NULL;
}

void octavia_fini(struct octavarium *octavia)
{
    if (octavia->interface.audio_fini)
        octavia->interface.audio_fini(octavia->interface.internal);
    zerop(octavia);
}

/** Backend entry point definitions for builds where such a backend is not supported. */
#define ENTRY_POINT_STUB(TYPE)                                                          \
    OCTAVIA_ENTRY_POINT(TYPE) {                                                         \
        (void)create_info;                                                              \
        *interface = (struct octavarium_interface){                                     \
            .id = octavia_backend_##TYPE,                                               \
            .name = string_init(#TYPE),                                                 \
        };                                                                              \
        if (verbose)                                                                    \
            log_error("Octavia: audio backend %u '%s' is not supported in this build.", \
                interface->id, interface->name.ptr);                                    \
        return result_error;                                                            \
    }

/* TODO to be implemented */
ENTRY_POINT_STUB(coreaudio)
ENTRY_POINT_STUB(wasapi)
ENTRY_POINT_STUB(xaudio2)
ENTRY_POINT_STUB(webaudio)
ENTRY_POINT_STUB(aaudio)
ENTRY_POINT_STUB(alsa)
ENTRY_POINT_STUB(jack)
ENTRY_POINT_STUB(pipewire)
ENTRY_POINT_STUB(pulseaudio)
ENTRY_POINT_STUB(oss)

#ifndef OCTAVIA_COREAUDIO
    /* TODO */
#endif
#ifndef OCTAVIA_WASAPI
    /* TODO */
#endif
#ifndef OCTAVIA_XAUDIO2
    /* TODO */
#endif
#ifndef OCTAVIA_WEBAUDIO
    /* TODO */
#endif
#ifndef OCTAVIA_AAUDIO
    /* TODO */
#endif
#ifndef OCTAVIA_PIPEWIRE
    /* TODO */
#endif
#ifndef OCTAVIA_PULSEAUDIO
    /* TODO */
#endif
#ifndef OCTAVIA_JACK
    /* TODO */
#endif
#ifndef OCTAVIA_ALSA
    /* TODO */
#endif
#ifndef OCTAVIA_OSS
    /* TODO */
#endif
#undef ENTRY_POINT_STUB

s32 octavia_entry_point(_OCTAVIA_ENTRY_POINT_ARGS)
{
    (void)interface; (void)create_info; (void)verbose;

    s32 res = result_error;
#define ENTRY_POINT_CHECK(name)                                          \
    res = octavia_##name##_entry_point(interface, create_info, verbose); \
    if (res == result_success || res == result_reiterate) return res;

#ifdef OCTAVIA_COREAUDIO
    ENTRY_POINT_CHECK(coreaudio)
#endif
#ifdef OCTAVIA_WASAPI
    ENTRY_POINT_CHECK(wasapi)
#endif
#ifdef OCTAVIA_XAUDIO2
    ENTRY_POINT_CHECK(xaudio2)
#endif
#ifdef OCTAVIA_WEBAUDIO
    ENTRY_POINT_CHECK(webaudio)
#endif
#ifdef OCTAVIA_AAUDIO
    ENTRY_POINT_CHECK(aaudio)
#endif
#ifdef OCTAVIA_PIPEWIRE
    ENTRY_POINT_CHECK(pipewire)
#endif
#ifdef OCTAVIA_PULSEAUDIO
    ENTRY_POINT_CHECK(pulseaudio)
#endif
#ifdef OCTAVIA_JACK
    ENTRY_POINT_CHECK(jack)
#endif
#ifdef OCTAVIA_ALSA
    ENTRY_POINT_CHECK(alsa)
#endif
#ifdef OCTAVIA_OSS
    ENTRY_POINT_CHECK(oss)
#endif

#undef ENTRY_POINT_CHECK
    interface->id = octavia_backend_invalid;
    interface->name = string_init("auto");
    return res;
}

s32 octavia_dummy_audio_init(
    struct octavarium                   *restrict octavia,
    const struct octavarium_create_info *restrict create_info)
{
    (void)octavia;
    (void)create_info;
    return result_success;
}

void octavia_dummy_audio_fini(void *restrict internal)
{
    (void)internal;
}

OCTAVIA_ENTRY_POINT(dummy) 
{
    (void)create_info; (void)verbose;

    *interface = (struct octavarium_interface){
        .id = octavia_backend_dummy,
        .name = string_init("dummy"),
        .internal = NULL,
        .audio_init = octavia_dummy_audio_init,
        .audio_fini = octavia_dummy_audio_fini,
    };
    return result_success;
}
