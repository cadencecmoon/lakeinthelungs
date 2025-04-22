if have_posix
    sources += files(
        'posix_mmap.c',
        'posix_module.c',
        'posix_threads.c',
        'posix_time.c',
    )
    defs += [ '-DLAKE_PLATFORM_UNIX=1', '-D_GNU_SOURCE']
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += '-DLAKE_HAS_CLOCK_GETTIME=1'
    endif

    dep_jack = dependency('jack', required : get_option('jack'))
    if dep_jack.found()
        #sources += files('')
        defs += '-DSOMA_JACK=1'
    endif
endif
