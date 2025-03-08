#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque handle of the audio backend. Can be safely cast into 
 *  (struct octavia_interface *) if obtained from a valid audio implementation. */
struct octavia;

/** Information needed to construct an audio backend. */
struct octavia_create_info {
    struct riven_create_info_header header;
};

#ifdef OCTAVIA_COREAUDIO
/** The entry point to an Apple CoreAudio backend. */
AMWAPI RIVEN_ENCORE(octavia, coreaudio);
#endif
#ifdef OCTAVIA_WASAPI
/** The entry point to a Windows WASAPI audio backend. */
AMWAPI RIVEN_ENCORE(octavia, wasapi);
#endif
#ifdef OCTAVIA_XAUDIO2
/** The entry point to a Windows XAudio2 backend. */
AMWAPI RIVEN_ENCORE(octavia, xaudio2);
#endif
#ifdef OCTAVIA_WEBAUDIO
/** The entry point to a Emscripten WebAudio backend. */
AMWAPI RIVEN_ENCORE(octavia, webaudio);
#endif
#ifdef OCTAVIA_AAUDIO
/** The entry point to a Android AAudio backend. */
AMWAPI RIVEN_ENCORE(octavia, aaudio);
#endif
#ifdef OCTAVIA_PIPEWIRE
/** The entry point to a Linux PipeWire audio backend. */
AMWAPI RIVEN_ENCORE(octavia, pipewire);
#endif
#ifdef OCTAVIA_PULSEAUDIO
/** The entry point to a Linux PulseAudio backend. */
AMWAPI RIVEN_ENCORE(octavia, pulseaudio);
#endif
#ifdef OCTAVIA_JACK
/** The entry point to a Unix JACK audio backend. */
AMWAPI RIVEN_ENCORE(octavia, jack);
#endif
#ifdef OCTAVIA_ALSA
/** The entry point to a Linux ALSA audio backend. */
AMWAPI RIVEN_ENCORE(octavia, alsa);
#endif
#ifdef OCTAVIA_OSS
/** The entry point to a Linux/Unix Open Sound System backend. */
AMWAPI RIVEN_ENCORE(octavia, oss);
#endif

/** The entry point to a dummy audio backend, available on all platforms.
 *  This is a valid API that is mute, but still can perform audio logic and 
 *  transforms on audio streams - can't record nor play them tho. */
AMWAPI RIVEN_ENCORE(octavia, dummy);

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
 *  This may be disabled by defining OCTAVIA_DISABLE_DUMMY_FALLBACK.
 *
 *  Other possible audio backends that could be created:
 *      Steinberg ASIO, FMOD, WWise, OpenAL, deprecated(OpenSL|ES, DirectSound, etc.)? */
AMWAPI RIVEN_ENCORE(octavia, native);

/** Procedures to be provided by an implementation. The backend must implement the 'struct octavia' and 
 *  put the octavia_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the audio backend and the public interface defined below. */
struct octavia_interface {
    struct riven_interface_header header;
};

#ifdef __cplusplus
}
#endif
