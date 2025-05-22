if with_platform_posix
    lake_sources += files(
        'dll.c',
        'mmap.c',
        'threads.c',
        'time.c')

    dl_dep = cc.find_library('dl', required: false)
    rt_dep = cc.find_library('rt', required: false)

    if cc.has_function('clock_gettime', dependencies: [rt_dep])
        pre_args += ['-DLAKE_HAS_CLOCK_GETTIME=1']
    endif

    pre_args += ['-DLAKE_PLATFORM_UNIX=1', '-D_GNU_SOURCE']
    lake_deps += [dl_dep, rt_dep]
endif
