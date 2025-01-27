audio_sources += files(
    'octavia.c',
)

dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('core'))
if dep_core.found()
    #audio_sources += files('')

    defs += '-DOCTAVIA_COREAUDIO=1'
    deps += dep_core
endif

found_wasapi = cc.check_header('audioclient.h', required : get_option('wasapi'))
if found_wasapi
    #audio_sources += files('')
    defs += '-DOCTAVIA_WASAPI=1'
    deps += cc.find_library('mfplat', required : true)
    deps += cc.find_library('mfuuid', required : true)
    deps += cc.find_library('ksuser', required : true)
    deps += cc.find_library('wmcodecdspuuid', required : true)
endif

dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
if dep_xaudio.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_XAUDIO2=1'
    #deps += dep_xaudio
endif

dep_aaudio = cc.find_library('aaudio', required : get_option('aaudio'))
if dep_aaudio.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_AAUDIO=1'
    #deps += dep_aaudio
endif

dep_alsa = dependency('alsa', required : get_option('alsa'))
if dep_alsa.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_ALSA=1'
    #deps += dep_alsa
endif

dep_jack = dependency('jack', required : get_option('jack'))
if dep_jack.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_JACK=1'
    #deps += dep_jack
endif

dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
if dep_pulse.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_PULSEAUDIO=1'
    #deps += dep_pulse
endif

dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
if dep_pipewire.found()
    #audio_sources += files('')
    defs += '-DOCTAVIA_PIPEWIRE=1'
    #deps += dep_pipewire
endif

found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
if found_oss
    #audio_sources += files('')
    defs += '-DOCTAVIA_OSS=1'
endif
