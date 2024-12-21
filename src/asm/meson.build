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
