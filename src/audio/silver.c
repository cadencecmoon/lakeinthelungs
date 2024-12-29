#include <lake/bedrock/log.h>
#include <lake/silver.h>

#ifndef AMW_NATIVE_COREAUDIO
AMWAPI bool AMWAPIENTRY silver_coreaudio_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The CoreAudio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_COREAUDIO */

#ifndef AMW_NATIVE_WASAPI
AMWAPI bool AMWAPIENTRY silver_wasapi_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The WASAPI backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_WASAPI */

#ifndef AMW_NATIVE_XAUDIO2
AMWAPI bool AMWAPIENTRY silver_xaudio2_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The XAudio2 backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_XAUDIO2 */

#ifndef AMW_NATIVE_AAUDIO
AMWAPI bool AMWAPIENTRY silver_aaudio_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The AAudio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_AAUDIO */

#ifndef AMW_NATIVE_ALSA
AMWAPI bool AMWAPIENTRY silver_alsa_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The ALSA audio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_ALSA */

#ifndef AMW_NATIVE_JACK
AMWAPI bool AMWAPIENTRY silver_jack_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The JACK audio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_JACK */

#ifndef AMW_NATIVE_PIPEWIRE
AMWAPI bool AMWAPIENTRY silver_pipewire_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The PipeWire audio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_PIPEWIRE */

#ifndef AMW_NATIVE_PULSEAUDIO
AMWAPI bool AMWAPIENTRY silver_pulse_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The PulseAudio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_PULSEAUDIO */

#ifndef AMW_NATIVE_OSS
AMWAPI bool AMWAPIENTRY silver_oss_entry_point(struct silver *silv)
{
    (void)silv;
    log_debug("The OSS audio backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_OSS */

AMWAPI bool AMWAPIENTRY silver_entry_point(struct silver *silv)
{
    if (silver_pipewire_entry_point(silv)) return true;
#if 0
    if (silver_coreaudio_entry_point(silv)) return true;
    if (silver_wasapi_entry_point(silv)) return true;
    if (silver_xaudio2_entry_point(silv)) return true;
    if (silver_aaudio_entry_point(silv)) return true;
    if (silver_alsa_entry_point(silv)) return true;
    if (silver_jack_entry_point(silv)) return true;
    if (silver_pulse_entry_point(silv)) return true;
    if (silver_oss_entry_point(silv)) return true;
    if (silver_dummy_entry_point(silv)) return true;
#endif
    return false;
}
