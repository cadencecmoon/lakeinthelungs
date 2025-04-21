#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* opaque handles, implemented by the backend */
struct soma_sink;
struct soma_encore;

struct soma_encore_assembly {
    u32 todo;
};

#ifdef SOMA_WASAPI
LAKEAPI FN_RIVEN_ENCORE(soma, wasapi);
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
LAKEAPI FN_RIVEN_ENCORE(soma, xaudio2);
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
LAKEAPI FN_RIVEN_ENCORE(soma, coreaudio);
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
LAKEAPI FN_RIVEN_ENCORE(soma, aaudio);
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
LAKEAPI FN_RIVEN_ENCORE(soma, webaudio);
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
LAKEAPI FN_RIVEN_ENCORE(soma, pipewire);
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
LAKEAPI FN_RIVEN_ENCORE(soma, pulse);
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
LAKEAPI FN_RIVEN_ENCORE(soma, jack);
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
LAKEAPI FN_RIVEN_ENCORE(soma, alsa);
#endif /* SOMA_ALSA */
#ifdef SOMA_OSS
LAKEAPI FN_RIVEN_ENCORE(soma, oss);
#endif /* SOMA_OSS */

/* A dummy audio backend, used for testing. */
LAKEAPI FN_RIVEN_ENCORE(soma, null);

/** Returns an array of native encores, with a predefined priority. */
LAKEAPI lake_nonnull(2) const PFN_riven_encore *LAKECALL 
soma_native_encores(bool null_fallback, u32 *out_encore_count);

#define SOMA_INTERFACE_SINK_HEADER \
    /** The encore used to open this sink. */ \
    struct soma_encore *encore;

enum soma_result {
    soma_result_success = 0,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
