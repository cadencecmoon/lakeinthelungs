#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* opaque handles, implemented by the backend */
struct soma_sink;
struct soma_encore;

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

struct soma_sink_header {
    struct soma_encore *encore;
};

#define ARGS_SOMA_SINK_QUERY(ENCORE) \
    struct soma_encore *ENCORE, \
    struct soma_sink  **default_playback, \
    struct soma_sink  **default_recording
typedef void (LAKECALL *PFN_soma_sink_query)(ARGS_SOMA_SINK_QUERY(encore));
#define FN_SOMA_SINK_QUERY(ENCORE) \
    void LAKECALL _soma_##ENCORE##_sink_query(ARGS_SOMA_SINK_QUERY(ENCORE))

typedef struct soma_sink *(LAKECALL *PFN_soma_sink_open)(struct soma_encore *encore);
#define FN_SOMA_SINK_OPEN(ENCORE) \
    struct soma_sink *LAKECALL _soma_##ENCORE##_sink_open(struct soma_encore *ENCORE)

typedef void (LAKECALL *PFN_soma_sink_close)(struct soma_sink *sink);
#define FN_SOMA_SINK_CLOSE(ENCORE) \
    void LAKECALL _soma_##ENCORE##_sink_close(struct soma_sink *sink)

typedef bool (LAKECALL *PFN_soma_sink_wait)(struct soma_sink *sink);
#define FN_SOMA_SINK_WAIT(ENCORE) \
    bool LAKECALL _soma_##ENCORE##_sink_wait(struct soma_sink *sink)

/** A public interface of the audio engine, implemented by a backend 'soma_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct soma_interface {
    struct riven_interface_header           header;

    PFN_soma_sink_query                     sink_query;
    PFN_soma_sink_open                      sink_open;
    PFN_soma_sink_close                     sink_close;
    PFN_soma_sink_wait                      sink_wait;
};

/** A view into the backend. */
union soma_encore_view {
    struct riven_interface_header          *header;
    struct soma_interface                  *interface;
    struct soma_encore                     *encore;
};

/** A view into an audio sink. */
union soma_sink_view {
    struct soma_sink_header                *header;
    struct soma_sink                       *data;
};

/** Implies different strategies for the audio engine. */
enum soma_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    soma_strategy_auto = 0,
    /** Runs a single primary backend. */
    soma_strategy_optimal,
    /** May run multiple audio backends. */
    soma_strategy_debug,
};

#ifndef SOMA_MAX_ENCORE_COUNT
    #define SOMA_MAX_ENCORE_COUNT 1
#endif /* SOMA_MAX_ENCORE_COUNT */

/** An engine structure for audio processing. */
struct soma {
    enum soma_strategy                      strategy;
    atomic_u32                              flags;
    union soma_encore_view                  backends[SOMA_MAX_ENCORE_COUNT];

    union soma_sink_view                   *playback_sinks;
    u32                                     playback_sink_count;
    u32                                     playback_sink_capacity;

    union soma_sink_view                   *recording_sinks;
    u32                                     recording_sink_count;
    u32                                     recording_sink_capacity;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
