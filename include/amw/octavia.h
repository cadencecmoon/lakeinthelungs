#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>
#include <amw/string.h>

struct octavia;

/** Numeric values to distinguish between the available audio backends. */
enum octavia_backend_id {
    octavia_backend_invalid = 0,
    octavia_backend_coreaudio,
    octavia_backend_wasapi,
    octavia_backend_xaudio2,
    octavia_backend_webaudio,
    octavia_backend_aaudio,
    octavia_backend_alsa,
    octavia_backend_jack,
    octavia_backend_pipewire,
    octavia_backend_pulseaudio,
    octavia_backend_oss,
    octavia_backend_dummy,
};

/** Defines an entry point for the audio backend. Verbose will enable log messages, including errors. */
typedef s32 (*PFN_octavia_entry_point)(struct octavia *octa, b32 verbose);

/** Implements the audio backend. */
struct octavia_interface {
    s32                         id;     /**< The numeric identifier of the display backend. */
    struct string               name;   /**< A readable name of the display backend, for logging. */
};

/** An audio engine. */
struct octavia {
    at_u32                      flags;
    rivens_tag_t                tag;
    struct rivens              *riven;

    void                       *backend;
    struct octavia_interface    interface;
};
