#include <amw/octavia.h>
#include <amw/log.h>

static const PFN_rivens_encore g_native_encores[] = {
#ifdef OCTAVIA_COREAUDIO
    (PFN_rivens_encore)octavia_coreaudio_encore,
#endif
#ifdef OCTAVIA_WASAPI
    (PFN_rivens_encore)octavia_wasapi_encore,
#endif
#ifdef OCTAVIA_XAUDIO2
    (PFN_rivens_encore)octavia_xaudio2_encore,
#endif
#ifdef OCTAVIA_WEBAUDIO
    (PFN_rivens_encore)octavia_webaudio_encore,
#endif
#ifdef OCTAVIA_AAUDIO
    (PFN_rivens_encore)octavia_aaudio_encore,
#endif
#ifdef OCTAVIA_PIPEWIRE
    (PFN_rivens_encore)octavia_pipewire_encore,
#endif
#ifdef OCTAVIA_PULSEAUDIO
    (PFN_rivens_encore)octavia_pulseaudio_encore,
#endif
#ifdef OCTAVIA_JACK
    (PFN_rivens_encore)octavia_jack_encore,
#endif
#ifdef OCTAVIA_ALSA
    (PFN_rivens_encore)octavia_alsa_encore,
#endif
#ifdef OCTAVIA_OSS
    (PFN_rivens_encore)octavia_oss_encore,
#endif
    (PFN_rivens_encore)octavia_dummy_encore,
};

const PFN_rivens_encore *octavia_acquire_native_encores(u32 *out_count, b32 fallback)
{
    u32 count = arraysize(g_native_encores);
    *out_count = fallback ? count : max(1, count-1);
    return g_native_encores;
}

b32 octavia_interface_validate(struct octavia *octavia)
{
    struct octavia_interface *interface = (struct octavia_interface *)octavia;
    const char *fmt = "Octavia '%s' is missing interface procedure - 'PFN_octavia_%s'.";
    b32 result = true;

#define CHECK(fn) \
    if (interface->fn == NULL) { log_warn(fmt, interface->header.name.ptr, #fn); result = false; }

    (void)interface;
    (void)fmt;

#undef CHECK
    return result;
}

static void dummy_interface_fini(struct octavia *octavia)
{
    (void)octavia;
}

RIVENS_ENCORE(octavia, dummy)
{
    struct octavia_interface *interface = (struct octavia_interface *) 
        riven_alloc(overture->header.riven, overture->header.tag, sizeof(struct octavia_interface), _Alignof(struct octavia_interface));

    *interface = (struct octavia_interface){
        .header = riven_write_interface_header(
            str_init("dummy"), 
            dummy_interface_fini, 
            octavia_interface_validate),
    };
    return (struct octavia *)interface;
}

/* XXX encore stubs, to be implemented */
#define ENCORE_STUB(backend) \
    RIVENS_ENCORE(octavia, backend) { log_error("Octavia encore '%s' is not yet implemented.", #backend); (void)overture; return NULL; }

#ifdef OCTAVIA_COREAUDIO
ENCORE_STUB(coreaudio)
#endif
#ifdef OCTAVIA_WASAPI
ENCORE_STUB(wasapi)
#endif
#ifdef OCTAVIA_XAUDIO2
ENCORE_STUB(xaudio2)
#endif
#ifdef OCTAVIA_WEBAUDIO
ENCORE_STUB(webaudio)
#endif
#ifdef OCTAVIA_AAUDIO
ENCORE_STUB(aaudio)
#endif
#ifdef OCTAVIA_PIPEWIRE
ENCORE_STUB(pipewire)
#endif
#ifdef OCTAVIA_PULSEAUDIO
ENCORE_STUB(pulseaudio)
#endif
#ifdef OCTAVIA_JACK
ENCORE_STUB(jack)
#endif
#ifdef OCTAVIA_ALSA
ENCORE_STUB(alsa)
#endif
#ifdef OCTAVIA_OSS
ENCORE_STUB(oss)
#endif
