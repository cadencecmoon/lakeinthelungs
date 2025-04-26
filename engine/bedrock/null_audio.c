#include <amwe/soma.h>

#ifdef SOMA_WASAPI
FN_RIVEN_ENCORE_STUB(soma, wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
FN_RIVEN_ENCORE_STUB(soma, xaudio)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
FN_RIVEN_ENCORE_STUB(soma, coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
FN_RIVEN_ENCORE_STUB(soma, aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
FN_RIVEN_ENCORE_STUB(soma, webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PULSEAUDIO
FN_RIVEN_ENCORE_STUB(soma, pulse)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
FN_RIVEN_ENCORE_STUB(soma, jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
FN_RIVEN_ENCORE_STUB(soma, alsa)
#endif /* SOMA_ALSA */
#ifdef SOMA_OSS
FN_RIVEN_ENCORE_STUB(soma, oss)
#endif /* SOMA_OSS */

/* TODO */
FN_RIVEN_ENCORE_STUB(soma, null)
