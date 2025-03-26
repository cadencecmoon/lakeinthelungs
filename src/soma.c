#include <amw/log.h>
#include <amw/soma.h>

RIVEN_ENCORE(soma, native)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#ifdef SOMA_COREAUDIO
        (PFN_riven_work)soma_encore_coreaudio,
#endif
#ifdef SOMA_WASAPI
        (PFN_riven_work)soma_encore_wasapi,
#endif
#ifdef SOMA_XAUDIO2
        (PFN_riven_work)soma_encore_xaudio2,
#endif
#ifdef SOMA_WEBAUDIO
        (PFN_riven_work)soma_encore_webaudio,
#endif
#ifdef SOMA_AAUDIO
        (PFN_riven_work)soma_encore_aaudio,
#endif
#ifdef SOMA_PIPEWIRE
        (PFN_riven_work)soma_encore_pipewire,
#endif
#ifdef SOMA_PULSEAUDIO
        (PFN_riven_work)soma_encore_pulseaudio,
#endif
#ifdef SOMA_JACK
        (PFN_riven_work)soma_encore_jack,
#endif
#ifdef SOMA_ALSA
        (PFN_riven_work)soma_encore_alsa,
#endif
#ifdef SOMA_OSS
        (PFN_riven_work)soma_encore_oss,
#endif
#ifndef SOMA_DISABLE_DUMMY_FALLBACK
        (PFN_riven_work)soma_encore_dummy,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i]((riven_argument_t)encore);

        if (*encore->header.interface == NULL) 
            continue;

        const struct soma_interface *interface = *encore->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_soma_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_debug(fmt, interface->header.name.ptr, #fn); valid = false; }
        VALIDATE(device_query)
        VALIDATE(device_open)
        VALIDATE(device_close)
        VALIDATE(device_wait)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((riven_argument_t)interface);
        *encore->header.interface = NULL;
    }
}

RIVEN_ENCORE(soma, dummy)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    struct soma_interface *interface = (struct soma_interface *) 
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct soma_interface), _Alignof(struct soma_interface));

    /* write the interface */
    interface->header.name = str_init("soma_dummy");
    interface->header.riven = encore->header.riven;
    interface->header.tag = encore->header.tag;
    interface->header.fini = (PFN_riven_work)riven_work_nop;

    *encore->header.interface = (riven_argument_t)(struct soma *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef SOMA_COREAUDIO
RIVEN_ENCORE_STUB(soma, coreaudio)
#endif
#ifdef SOMA_WASAPI
RIVEN_ENCORE_STUB(soma, wasapi)
#endif
#ifdef SOMA_XAUDIO2
RIVEN_ENCORE_STUB(soma, xaudio2)
#endif
#ifdef SOMA_WEBAUDIO
RIVEN_ENCORE_STUB(soma, webaudio)
#endif
#ifdef SOMA_AAUDIO
RIVEN_ENCORE_STUB(soma, aaudio)
#endif
#ifdef SOMA_PULSEAUDIO
RIVEN_ENCORE_STUB(soma, pulseaudio)
#endif
#ifdef SOMA_JACK
RIVEN_ENCORE_STUB(soma, jack)
#endif
#ifdef SOMA_ALSA
RIVEN_ENCORE_STUB(soma, alsa)
#endif
#ifdef SOMA_OSS
RIVEN_ENCORE_STUB(soma, oss)
#endif
