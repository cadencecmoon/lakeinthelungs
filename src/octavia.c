#include <amw/octavia.h>
#include <amw/log.h>

RIVEN_ENCORE(octavia, native)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    PFN_riven_work encores[] = {
#ifdef OCTAVIA_COREAUDIO
        (PFN_riven_work)octavia_encore_coreaudio,
#endif
#ifdef OCTAVIA_WASAPI
        (PFN_riven_work)octavia_encore_wasapi,
#endif
#ifdef OCTAVIA_XAUDIO2
        (PFN_riven_work)octavia_encore_xaudio2,
#endif
#ifdef OCTAVIA_WEBAUDIO
        (PFN_riven_work)octavia_encore_webaudio,
#endif
#ifdef OCTAVIA_AAUDIO
        (PFN_riven_work)octavia_encore_aaudio,
#endif
#ifdef OCTAVIA_PIPEWIRE
        (PFN_riven_work)octavia_encore_pipewire,
#endif
#ifdef OCTAVIA_PULSEAUDIO
        (PFN_riven_work)octavia_encore_pulseaudio,
#endif
#ifdef OCTAVIA_JACK
        (PFN_riven_work)octavia_encore_jack,
#endif
#ifdef OCTAVIA_ALSA
        (PFN_riven_work)octavia_encore_alsa,
#endif
#ifdef OCTAVIA_OSS
        (PFN_riven_work)octavia_encore_oss,
#endif
#ifndef OCTAVIA_DISABLE_DUMMY_FALLBACK
        (PFN_riven_work)octavia_encore_dummy,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i]((riven_argument_t)create_info);

        if (*create_info->header.interface == NULL) 
            continue;

        const struct octavia_interface *interface = *create_info->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_octavia_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_warn(fmt, interface->header.name.ptr, #fn); valid = false; }
        (void)fmt;
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((riven_argument_t)interface);
        *create_info->header.interface = NULL;
    }
}

RIVEN_ENCORE(octavia, dummy)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    struct riven *riven = create_info->header.riven;
    riven_tag_t tag = create_info->header.tag;

    struct octavia_interface *interface = (struct octavia_interface *) 
        riven_alloc(riven, tag, sizeof(struct octavia_interface), _Alignof(struct octavia_interface));

    *interface = (struct octavia_interface){
        .header = {
            .name = str_init("octavia_dummy"),
            .riven = riven,
            .tag = tag,
            .fini = riven_work_nop,
        },
    };
    *create_info->header.interface = (riven_argument_t)(struct octavia *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef OCTAVIA_COREAUDIO
RIVEN_ENCORE_STUB(octavia, coreaudio)
#endif
#ifdef OCTAVIA_WASAPI
RIVEN_ENCORE_STUB(octavia, wasapi)
#endif
#ifdef OCTAVIA_XAUDIO2
RIVEN_ENCORE_STUB(octavia, xaudio2)
#endif
#ifdef OCTAVIA_WEBAUDIO
RIVEN_ENCORE_STUB(octavia, webaudio)
#endif
#ifdef OCTAVIA_AAUDIO
RIVEN_ENCORE_STUB(octavia, aaudio)
#endif
#ifdef OCTAVIA_PIPEWIRE
RIVEN_ENCORE_STUB(octavia, pipewire)
#endif
#ifdef OCTAVIA_PULSEAUDIO
RIVEN_ENCORE_STUB(octavia, pulseaudio)
#endif
#ifdef OCTAVIA_JACK
RIVEN_ENCORE_STUB(octavia, jack)
#endif
#ifdef OCTAVIA_ALSA
RIVEN_ENCORE_STUB(octavia, alsa)
#endif
#ifdef OCTAVIA_OSS
RIVEN_ENCORE_STUB(octavia, oss)
#endif
