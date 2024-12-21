#ifndef _AMW_NIGHTINGALE_H
#define _AMW_NIGHTINGALE_H

#include <lake/defines.h>

/** Audio backends are using native APIs of different platforms. */
enum nightingale_api {
    nightingale_api_auto = 0u,
    nightingale_api_coreaudio,
    nightingale_api_wasapi,
    nightingale_api_xaudio2,
    nightingale_api_aaudio,
    nightingale_api_alsa,
    nightingale_api_jack,
    nightingale_api_pipewire,
    nightingale_api_pulseaudio,
    nightingale_api_oss,
    nightingale_api_dummy, /* no sound, but functional api */
    nightingale_api_count,
};

/** Opaque state for the audio engine context. */
struct nightingale;

#endif /* _AMW_NIGHTINGALE_H */
