# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

if sys == 'windows'
    #engine = files('')

    defs += [
        '-DAMW_PLATFORM_WINDOWS',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]
    deps += dep_threads
    deps += cc.find_library('ole32', required: true)
	deps += cc.find_library('winmm', required: true)
    summary_sys_apis += [ 'Win32' ]
endif
