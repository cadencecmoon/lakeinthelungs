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
else # 32-bit
    error('32-bit x86 platforms not fully supported right now')
endif
