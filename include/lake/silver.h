#ifndef _AMW_SILVER_H
#define _AMW_SILVER_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#include <lake/ipomoeaalba.h>

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
 *  - Apple: TODO CoreAudio
 *  - Windows: TODO WASAPI, XAudio2
 *  - Android: TODO AAudio
 *  - Unix/Linux: TODO JACK, OSS
 *  - Linux: TODO ALSA, PulseAudio, PipeWire
 *  - Dummy (no api, validation and debugging)
 *
 *  Creating a possible backend from middleware like OpenAL, Wwise or FMOD, or other proprietary 
 *  APIs like Steinberg ASIO, should be straightforward if an application wishes to support them. */
typedef int32_t (AMWAPIENTRY *PFN_silver_entry_point)(struct silver *silv, struct ipomoeaalba *ia);

#ifndef AMW_NO_PROTOTYPES
AMWAPI int32_t AMWAPIENTRY silver_pipewire_entry_point(struct silver *silv, struct ipomoeaalba *ia);
AMWAPI int32_t AMWAPIENTRY silver_dummy_entry_point(struct silver *silv, struct ipomoeaalba *ia);
/** Select the first appropriate audio backend supported by this build. */
AMWAPI int32_t AMWAPIENTRY silver_entry_point(struct silver *silv, struct ipomoeaalba *ia);
#endif /* AMW_NO_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_SILVER_H */
