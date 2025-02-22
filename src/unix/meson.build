if have_posix
    engine_sources += files(
        'posix_plugin.c',
        'posix_poll.c',
        'posix_threads.c',
        'posix_time.c',
    )

    defs += [ '-DPLATFORM_UNIX=1', '-D_GNU_SOURCE']
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += '-DHAS_CLOCK_GETTIME=1'
    endif

    dep_jack = dependency('jack', required : get_option('jack'))
    if dep_jack.found()
        #engine_sources += files()
        defs += '-DOCTAVIA_JACK=1'
    endif

    found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
    if found_oss
        #engine_sources += files()
        defs += '-DOCTAVIA_OSS=1'
    endif
endif
