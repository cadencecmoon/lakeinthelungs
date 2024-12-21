# CoreAudio
dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('core'))
if dep_core.found()
    #engine += files('')

    defs += '-DAMW_NATIVE_COREAUDIO=1'
    deps += dep_core
endif

# WASAPI
found_wasapi = cc.check_header('audioclient.h', required : get_option('wasapi'))
if found_wasapi
    #engine += files('')

    defs += '-DAMW_NATIVE_WASAPI=1'
    deps += cc.find_library('mfplat', required : true)
    deps += cc.find_library('mfuuid', required : true)
    deps += cc.find_library('ksuser', required : true)
    deps += cc.find_library('wmcodecdspuuid', required : true)
endif

# XAudio2
dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
if dep_xaudio.found()
    #engine += files('')
    
    defs += '-DAMW_NATIVE_XAUDIO2=1'
    #deps += dep_xaudio
endif

# AAudio
dep_aaudio = cc.find_library('aaudio', required : get_option('aaudio'))
if dep_aaudio.found()
    #engine += files('')

    defs += '-DAMW_NATIVE_AAUDIO=1'
    #deps += dep_aaudio
endif

# ALSA
dep_alsa = dependency('alsa', required : get_option('alsa'))
if dep_alsa.found()
    #engine += files('')

    defs += '-DAMW_NATIVE_ALSA=1'
    #deps += dep_alsa
endif

# JACK
dep_jack = dependency('jack', required : get_option('jack'))
if dep_jack.found()
    #engine += files('')

    defs += '-DAMW_NATIVE_JACK=1'
    #deps += dep_jack
endif

# PulseAudio
dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
if dep_pulse.found()
    #engine += files('')

    defs += '-DAMW_NATIVE_PULSEAUDIO=1'
    #deps += dep_pulse
endif

# PipeWire
dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
if dep_pipewire.found()
    #negine += files('')

    defs += [ 
        '-DAMW_NATIVE_PIPEWIRE=1', 
        '-D_GNU_SOURCE' ]
    #deps += dep_pipewire
endif

# OSS
found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
if found_oss
    #sources += files('')

    defs += '-DAMW_NATIVE_OSS=1'
endif
