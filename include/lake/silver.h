#ifndef _AMW_SILVER_H
#define _AMW_SILVER_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#include <lake/ipomoeaalba.h>

#ifdef __cplusplus
extern "C" {
#endif

struct silver;

/** Entry point to the Silver audio backend. 
 *  @return True if loading the backend module and procedures was successful.
 *  Creating a possible backend from middleware like OpenAL, Wwise or FMOD, or other proprietary 
 *  APIs like Steinberg ASIO, should be straightforward if an application wishes to support them. */
typedef s32 (*PFN_silver_entry_point)(struct silver *silv, struct ipomoeaalba *ia);

/** An enumeration to identify the API used for accessing audio on the system.
 *  This can be either native audio drivers, APIs of audio servers or audio middleware.
 *  The backends are used for low-level audio control, to implement a common audio access 
 *  point for different platforms, or to in other ways accelerate the audio processing. */
enum silver_backend_api {
    silver_backend_api_coreaudio = 0,   // TODO reserved id
    silver_backend_api_wasapi,          // TODO reserved id
    silver_backend_api_xaudio2,         // TODO reserved id
    silver_backend_api_aaudio,          // TODO reserved id
    silver_backend_api_alsa,            // TODO reserved id
    silver_backend_api_jack,            // TODO reserved id
    silver_backend_api_pulseaudio,      // TODO reserved id
    silver_backend_api_pipewire,        // TODO reserved id
    silver_backend_api_oss,             // TODO reserved id
    silver_backend_api_dummy,
    silver_backend_api_custom = 0x20,
};

AMWAPI s32 silver_pipewire_entry_point(struct silver *silv, struct ipomoeaalba *ia);
AMWAPI s32 silver_dummy_entry_point(struct silver *silv, struct ipomoeaalba *ia);
/** Select the first appropriate audio backend supported by this build. */
AMWAPI s32 silver_entry_point(struct silver *silv, struct ipomoeaalba *ia);

/** A context for the audio backend. Internally uses native APIs and audio servers. */
struct silver {
    at_u32      flags;          /**< Any write-modify operations outside the backend must be externally synchronized. */

    u32         backend_api;    /**< Either enum silver_backend_api or a custom identifier. */
    const char *backend_name;   /**< A readable name of the running audio backend, for logging purposes. */
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_SILVER_H */
