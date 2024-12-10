# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

if have_posix
    engine += files(
        'posix_threads.c',
        'posix_time.c',
    )
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += [ '-DAMW_HAS_CLOCK_GETTIME=1']
    endif

    defs += '-DAMW_PLATFORM_UNIX=1'
endif
