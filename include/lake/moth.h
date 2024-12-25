#ifndef _AMW_MOTH_H
#define _AMW_MOTH_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Audio backends are using native APIs of different platforms. */
enum moth_backend {
    moth_backend_auto = 0u,
    moth_backend_coreaudio,     /* TODO */
    moth_backend_wasapi,        /* TODO */
    moth_backend_xaudio2,       /* TODO */
    moth_backend_aaudio,        /* TODO */
    moth_backend_alsa,          /* TODO */
    moth_backend_jack,          /* TODO */
    moth_backend_pipewire,      /* TODO */
    moth_backend_pulseaudio,    /* TODO */
    moth_backend_oss,           /* TODO */
    moth_backend_dummy,         /* no sound, but functional api for testing */
    moth_backend_count,
};

/** Opaque state for the audio engine context. */
struct moth;

#ifdef __cplusplus
}
#endif

#endif /* _AMW_MOTH_H */
