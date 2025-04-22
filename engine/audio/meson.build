sources += files(
    'null_audio.c',
    'soma.c',
)

dep_vorbis = dependency('vorbis', fallback : ['vorbis', 'dep_vorbis'])
if dep_vorbis.found()
    deps += dep_vorbis
    defs += '-DSOMA_VORBIS=1'
endif

# TODO asio sdk
asio_found = false
if asio_found
    sources += files(
        'iasiothiscallresolver.cpp',
    )
    defs += '-DSOMA_ASIO=1'
endif

wasapi_found = cc.check_header('audioclient.h', required : get_option('wasapi'))
if wasapi_found
    #sources += files('')
    defs += '-DSOMA_WASAPI=1'
endif

dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
if dep_xaudio.found()
    #sources += files('')
    defs += '-DSOMA_XAUDIO2=1'
endif

dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('coreaudio'))
if dep_core.found()
    #sources += files('')
    defs += '-DSOMA_COREAUDIO=1'
    deps += dep_core
endif

dep_aaudio = cc.find_library('aaudio', required : get_option('aaudio'))
if dep_aaudio.found()
    #sources += files()
    defs += '-DSOMA_AAUDIO=1'
endif

dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
if dep_pipewire.found()
    sources += files('pipewire.c')
    defs += '-DSOMA_PIPEWIRE=1'
endif

dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
if dep_pulse.found()
    #sources += files('')
    defs += '-DSOMA_PULSEAUDIO=1'
endif

dep_jack = dependency('jack', required : get_option('jack'))
if dep_jack.found()
    #sources += files('')
    defs += '-DSOMA_JACK=1'
endif

dep_alsa = dependency('alsa', required : get_option('alsa'))
if dep_alsa.found()
    #sources += files('')
    defs += '-DSOMA_ALSA=1'
endif

found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
if found_oss
    #sources += files()
    defs += '-DSOMA_OSS=1'
endif
