if have_posix
    bedrock_sources += files(
        'dlfcn.c',
        'pthread.c',
        'time.c',
    )
    defs += [ '-DPLATFORM_UNIX=1', '-D_GNU_SOURCE']
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += '-DHAS_CLOCK_GETTIME=1'
    endif
endif
