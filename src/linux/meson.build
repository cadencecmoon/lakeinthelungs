if sys == 'android' or sys == 'linux' or sys == 'bsd'
    engine_sources += files(
        'linux_process.c',
    )
    subdir('drmkms')
    defs += '-DPLATFORM_LINUX=1'
endif

if sys == 'linux' or sys == 'bsd'
    subdir('wayland')
    subdir('xcb')

    dep_alsa = dependency('alsa', required : get_option('alsa'))
    if dep_alsa.found()
        #engine_sources += files('')
        defs += '-DSOMA_ALSA=1'
    endif

    dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
    if dep_pulse.found()
        #engine_sources += files('')
        defs += '-DSOMA_PULSEAUDIO=1'
    endif

    dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
    if dep_pipewire.found()
        engine_sources += files('pipewire.c')
        defs += '-DSOMA_PIPEWIRE=1'
    endif
endif
