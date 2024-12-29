#ifndef _AMW_SILVER_H
#define _AMW_SILVER_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A context for the audio backend. Internally uses native APIs and audio servers. */
struct silver {
    at_uint32_t flags;
    void *internal;
};

/** Entry point to the Silver audio backend. 
 *  @return True if loading the backend module and procedures was successful.
 *
 *  Available audio backends in A Moonlit Walk are: 
 *  - Apple: CoreAudio
 *  - Windows: WASAPI, XAudio2
 *  - Android: AAudio
 *  - Unix/Linux: JACK, OSS
 *  - Linux: ALSA, PulseAudio, PipeWire
 *  - Dummy (no api, validation and debugging) */
typedef bool (AMWAPIENTRY *PFN_silver_entry_point)(struct silver *silv);

#ifndef AMW_NO_PROTOTYPES
AMWAPI bool AMWAPIENTRY silver_coreaudio_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_wasapi_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_xaudio2_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_aaudio_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_alsa_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_jack_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_pipewire_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_pulse_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_oss_entry_point(struct silver *silv);
AMWAPI bool AMWAPIENTRY silver_dummy_entry_point(struct silver *silv);
/** Select the first appropriate audio backend supported by this build. */
AMWAPI bool AMWAPIENTRY silver_entry_point(struct silver *silv);
#endif /* AMW_NO_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_SILVER_H */
