#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

struct octavarium;
struct octavarium_interface;
struct octavarium_create_info;

/** Arguments of the entry point to the audio backend. */
#define _OCTAVIA_ENTRY_POINT_ARGS                              \
    struct octavarium_interface         *restrict interface,   \
    const struct octavarium_create_info *restrict create_info

/** Defines an entry point for the audio backend. Verbose will enable log messages, including errors. */
typedef enum result attr_nonnull_all (*PFN_octavia_entry_point)(_OCTAVIA_ENTRY_POINT_ARGS);

/** Declares a pelagial entry point implementation. */
#define OCTAVIA_ENTRY_POINT(name) \
    s32 attr_nonnull_all octavia_##name##_entry_point(_OCTAVIA_ENTRY_POINT_ARGS)

/* Predefined audio backends available. */
AMWAPI OCTAVIA_ENTRY_POINT(coreaudio);  /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(wasapi);     /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(xaudio2);    /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(webaudio);   /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(aaudio);     /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(alsa);       /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(jack);       /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(pipewire);   /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(pulseaudio); /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(oss);        /* TODO reserved */
AMWAPI OCTAVIA_ENTRY_POINT(dummy);

/** Picks the first valid entry point from the available above. */
AMWAPI enum result octavia_entry_point(_OCTAVIA_ENTRY_POINT_ARGS);

/** Initializes the audio backend. */
typedef enum result (*PFN_octavia_audio_init)(
    struct octavarium                   *octavia,
    const struct octavarium_create_info *create_info);

/** Cleanups the audio backend. */
typedef void (*PFN_octavia_audio_fini)(void *internal);

/** Procedures implemented by the audio backend. */
struct octavarium_interface {
    const char                 *name;       /**< A readable name of the audio backend, for logging. */
    void                       *internal;   /**< The audio backend. */

    PFN_octavia_audio_init      audio_init;
    PFN_octavia_audio_fini      audio_fini;
};

/** An context of the audio engine. */
struct octavarium {
    at_u32                      flags;
    struct octavarium_interface interface;

    rivens_tag_t                tag;    /**< The lifetime of this system. */
    struct rivens              *riven;
};

/** Information needed to construct an audio backend. */
struct octavarium_create_info {
    struct rivens          *riven;
    rivens_tag_t            tag;
};

/** Initialize the backend instance for the renderer. */
AMWAPI attr_nonnull_all
struct octavarium *octavia_init(
    PFN_octavia_entry_point       *entries,
    u32                            entry_count,
    struct octavarium_create_info *create_info);

AMWAPI attr_nonnull_all
void octavia_fini(struct octavarium *octavia);

#ifdef __cplusplus
}
#endif
