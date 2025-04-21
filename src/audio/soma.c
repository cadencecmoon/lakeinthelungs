#include <amwe/audio/soma.h>

static const PFN_riven_encore native_encores[] = {
#ifdef SOMA_WASAPI
    (PFN_riven_encore)soma_encore_wasapi,
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
    (PFN_riven_encore)soma_encore_xaudio2,
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
    (PFN_riven_encore)soma_encore_coreaudio,
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
    (PFN_riven_encore)soma_encore_aaudio,
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
    (PFN_riven_encore)soma_encore_webaudio,
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
    (PFN_riven_encore)soma_encore_pipewire,
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
    (PFN_riven_encore)soma_encore_pulse,
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
    (PFN_riven_encore)soma_encore_jack,
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
    (PFN_riven_encore)soma_encore_alsa,
#endif /* SOMA_ALSA */
#ifdef SOMA_OSS
    (PFN_riven_encore)soma_encore_oss,
#endif /* SOMA_OSS */
    (PFN_riven_encore)soma_encore_null,
};

const PFN_riven_encore *soma_native_encores(bool null_fallback, u32 *out_encore_count)
{
    u32 encore_count = lake_arraysize(native_encores);
    if (encore_count > 1 && !null_fallback)
        encore_count--;

    *out_encore_count = encore_count;
    return native_encores;
}

extern FN_RIVEN_INTERFACE_VALIDATION(soma)
{
    const char *fmt = "'%s: %s' is missing an interface procedure - 'PFN_soma_%s'.";
    bool valid = true;

#define VALIDATE(FN) \
    if (interface->FN == NULL) { bedrock_log_debug(fmt, interface->header.name, interface->header.backend, #FN); valid = false; }

    VALIDATE(sink_query)
    VALIDATE(sink_open)
    VALIDATE(sink_close)
    VALIDATE(sink_wait)
#undef VALIDATE

    return valid;
}

#ifdef SOMA_WASAPI
FN_RIVEN_ENCORE_STUB(soma, wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
FN_RIVEN_ENCORE_STUB(soma, xaudio)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
FN_RIVEN_ENCORE_STUB(soma, coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
FN_RIVEN_ENCORE_STUB(soma, aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
FN_RIVEN_ENCORE_STUB(soma, webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PULSEAUDIO
FN_RIVEN_ENCORE_STUB(soma, pulse)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
FN_RIVEN_ENCORE_STUB(soma, jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
FN_RIVEN_ENCORE_STUB(soma, alsa)
#endif /* SOMA_ALSA */
#ifdef SOMA_OSS
FN_RIVEN_ENCORE_STUB(soma, oss)
#endif /* SOMA_OSS */

FN_RIVEN_ENCORE_STUB(soma, null)

void soma_fini(struct soma *soma)
{
    (void)soma;
}
