#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

struct soma_device;
/** An opaque handle of the audio backend. Can be safely cast into 
 *  (struct soma_interface *) if obtained from a valid audio implementation. */
struct soma;

/** Information needed to construct an audio backend. */
struct soma_encore {
    struct riven_encore_header header;
};

#ifdef SOMA_COREAUDIO
/** The entry point to an Apple CoreAudio backend. */
AMWAPI RIVEN_ENCORE(soma, coreaudio);
#endif
#ifdef SOMA_WASAPI
/** The entry point to a Windows WASAPI audio backend. */
AMWAPI RIVEN_ENCORE(soma, wasapi);
#endif
#ifdef SOMA_XAUDIO2
/** The entry point to a Windows XAudio2 backend. */
AMWAPI RIVEN_ENCORE(soma, xaudio2);
#endif
#ifdef SOMA_WEBAUDIO
/** The entry point to a Emscripten WebAudio backend. */
AMWAPI RIVEN_ENCORE(soma, webaudio);
#endif
#ifdef SOMA_AAUDIO
/** The entry point to a Android AAudio backend. */
AMWAPI RIVEN_ENCORE(soma, aaudio);
#endif
#ifdef SOMA_PIPEWIRE
/** The entry point to a Linux PipeWire audio backend. */
AMWAPI RIVEN_ENCORE(soma, pipewire);
#endif
#ifdef SOMA_PULSEAUDIO
/** The entry point to a Linux PulseAudio backend. */
AMWAPI RIVEN_ENCORE(soma, pulseaudio);
#endif
#ifdef SOMA_JACK
/** The entry point to a Unix JACK audio backend. */
AMWAPI RIVEN_ENCORE(soma, jack);
#endif
#ifdef SOMA_ALSA
/** The entry point to a Linux ALSA audio backend. */
AMWAPI RIVEN_ENCORE(soma, alsa);
#endif
#ifdef SOMA_OSS
/** The entry point to a Linux/Unix Open Sound System backend. */
AMWAPI RIVEN_ENCORE(soma, oss);
#endif

/** The entry point to a dummy audio backend, available on all platforms.
 *  This is a valid API that is mute, but still can perform audio logic and 
 *  transforms on audio streams - can't record nor play them tho. */
AMWAPI RIVEN_ENCORE(soma, dummy);

/** Executes native encores until one writes to the interface:
 *  - coreaudio - Apple platforms (MacOS, iOS).
 *  - wasapi, xaudio2 - Windows platforms (Windows, XBox).
 *  - webaudio - WebAssembly builds.
 *  - aaudio - Android builds.
 *  - pipewire, pulseaudio, alsa - Linux builds.
 *  - jack - Linux/Unix/MacOS builds.
 *  - oss - Linux/Unix(BSD,Solaris,etc.) builds.
 *
 *  Available encores are tried in order:
 *      Apple: coreaudio -> jack
 *      Windows: wasapi -> xaudio2,
 *      Linux/Unix: pipewire -> pulseaudio -> jack -> alsa -> oss
 *      Other platforms their only native option (aaudio, webaudio)
 *
 *  If all options fail, the dummy interface may be written as a fallback.
 *  This may be disabled by defining SOMA_DISABLE_DUMMY_FALLBACK.
 *
 *  Other possible audio backends that could be created:
 *      Steinberg ASIO, FMOD, WWise, OpenAL, deprecated(OpenSL|ES, DirectSound, etc.)? */
AMWAPI RIVEN_ENCORE(soma, native);

struct soma_device_header {
    struct soma *soma;
};

#define ARGS_SOMA_DEVICE_QUERY                          \
    struct soma                    *soma,               \
    struct soma_device            **default_playback,   \
    struct soma_device            **default_recording
typedef void (AMWCALL *PFN_soma_device_query)(ARGS_SOMA_DEVICE_QUERY);
#define FN_SOMA_DEVICE_QUERY(encore) \
    extern void AMWCALL _soma_##encore##_device_query(ARGS_SOMA_DEVICE_QUERY)

#define ARGS_SOMA_DEVICE_OPEN               \
    struct soma                    *soma,   \
    struct riven_memory            *memory, \
    struct soma_device            **out_device
typedef s32 (AMWCALL *PFN_soma_device_open)(ARGS_SOMA_DEVICE_OPEN);
#define FN_SOMA_DEVICE_OPEN(encore) \
    extern s32 AMWCALL _soma_##encore##_device_open(ARGS_SOMA_DEVICE_OPEN)

typedef void (AMWCALL *PFN_soma_device_close)(struct soma_device *device);
#define FN_SOMA_DEVICE_CLOSE(encore) \
    extern void AMWCALL _soma_##encore##_device_close(struct soma_device *device)

typedef b32 (AMWCALL *PFN_soma_device_wait)(struct soma_device *device);
#define FN_SOMA_DEVICE_WAIT(encore) \
    extern b32 AMWCALL _soma_##encore##_device_wait(struct soma_device *device)

/** Procedures to be provided by an implementation. The backend must implement the 'struct soma' and 
 *  put the octavia_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the audio backend and the public interface defined below. */
struct soma_interface {
    struct riven_interface_header   header;

    PFN_soma_device_query           device_query;
    PFN_soma_device_open            device_open;
    PFN_soma_device_close           device_close;
    PFN_soma_device_wait            device_wait;
};

#ifdef __cplusplus
}
#endif
