#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef SOMA_INTERFACE_MAX_DEVICES_COUNT
#define SOMA_INTERFACE_MAX_DEVICES_COUNT 4
#endif

/* opaque handles, implemented by the backend */
struct soma_device;
struct soma_encore;

/******************************
 *                            * 
 *     INTERNAL INTERFACE     * 
 *                            * 
 ******************************/

#define SOMA_INTERFACE_DEVICE_HEADER \
    /** The encore used to open this device. */ \
    struct soma_encore *soma;

typedef lake_nodiscard enum lake_result (LAKECALL *PFN_soma_device_query)(struct soma_encore *ENCORE, struct soma_device **default_playback, struct soma_device **default_recording);
#define FN_SOMA_DEVICE_QUERY(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _soma_##ENCORE##_device_query(struct soma_encore *ENCORE, struct soma_device **default_playback, struct soma_device **default_recording)

typedef lake_nodiscard enum lake_result (LAKECALL *PFN_soma_device_open)(struct soma_encore *encore);
#define FN_SOMA_DEVICE_OPEN(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _soma_##ENCORE##_device_open(struct soma_encore *ENCORE)

typedef void (LAKECALL *PFN_soma_device_close)(struct soma_device *device);
#define FN_SOMA_DEVICE_CLOSE(ENCORE) \
    void LAKECALL _soma_##ENCORE##_device_close(struct soma_device *device)

typedef bool (LAKECALL *PFN_soma_device_wait)(struct soma_device *device);
#define FN_SOMA_DEVICE_WAIT(ENCORE) \
    bool LAKECALL _soma_##ENCORE##_device_wait(struct soma_device *device)

/******************************
 *                            * 
 *      PUBLIC INTERFACE      * 
 *                            * 
 ******************************/

/** A view into an audio device. */
union soma_device_view {
    struct {SOMA_INTERFACE_DEVICE_HEADER}  *header;
    struct soma_device                     *device;
    union soma_audio                       *soma;
};

/** A public interface of the audio engine, implemented by a backend 'soma_encore'. */
struct soma_interface {
    struct riven_interface              riven;

    union soma_device_view              playback_devices[SOMA_INTERFACE_MAX_DEVICES_COUNT];
    union soma_device_view              record_devices[SOMA_INTERFACE_MAX_DEVICES_COUNT];
    u32                                 playback_device_count;
    u32                                 record_device_count;

    /* procedures, implemented by the backend */
    PFN_soma_device_query               device_query;
    PFN_soma_device_open                device_open;
    PFN_soma_device_close               device_close;
    PFN_soma_device_wait                device_wait;
};
union soma_audio {
    struct riven_interface     *riven;
    struct soma_interface      *interface;
    struct soma_encore         *encore;
    void                       *data;
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
