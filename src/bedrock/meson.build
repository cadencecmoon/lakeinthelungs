# Lake in the Lungs
# Copyright (c) 2025 Cadence C. Moon
# The source code is licensed under a standard MIT license.

engine += files(
    'log.c',
    'ticks.c',
)

if host_machine.cpu_family() == 'x86_64'
    # The baseline (x86_64-v1): MMX, SSE and SSE2 instructions, are implemented by all x86_64 CPUs

    # Resolve additional intrinsics options, that we can make use of
    if get_option('avx')
        defs += '-mavx'
    endif
    if get_option('avx2')
        defs += [ '-mavx2', '-mfma' ]
    endif

    # Resolve ABI of different supported platforms
    if sys == 'windows'
        subdir('amd64_pe')
    elif sys == 'darwin'
        subdir('amd64_macho')
    else
        subdir('amd64_elf')
    endif
elif host_machine.cpu_family() == 'x86' 
    # TODO i686 elf macho pe
elif host_machine.cpu_family() == 'arm'
    # TODO armv7a elf macho 
elif host_machine.cpu_family() == 'aarch64'
    # TODO aarch64 elf macho pe
elif host_machine.cpu_family() == 'riscv32'
    # TODO rv32gc elf
elif host_machine.cpu_family() == 'riscv64'
    # TODO rv64gc elf
else
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not yet supported.')
    # WebAssembly ?
endif

dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

# POSIX / UNIX
if have_posix
    engine += files(
        'unix/dynamic_lib.c',
        'unix/posix_threads.c',
        'unix/posix_timer.c',
    )
    defs += [ '-DAMW_PLATFORM_UNIX=1', '-D_GNU_SOURCE']
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += '-DAMW_HAS_CLOCK_GETTIME=1'
    endif
endif

# Windows
if sys == 'windows'
    #engine = files('')

    defs += [
        '-DAMW_PLATFORM_WINDOWS=1',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]
    deps += dep_threads

# Apple MacOS/iOS
elif sys == 'darwin'
    #engine = files('')

    defs += [
        '-DAMW_PLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    add_languages('objc')

    if host_machine.subsystem() == 'ios'
        defs += '-DAMW_PLATFORM_IOS=1'
    else
        defs += '-DAMW_PLATFORM_MACOS=1'
    endif

# Linux / BSD / Android
elif sys == 'android' or sys == 'linux' or sys == 'bsd'
    engine += files(
        'linux/cpu.c',
    )

    defs += '-DAMW_PLATFORM_LINUX=1'
endif
