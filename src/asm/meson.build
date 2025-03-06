assembly_arch = []
assembly_source = []

if sys == 'windows'
    assembly_source = 'pe.asm'
elif sys == 'darwin'
    assembly_source = 'macho.s'
else
    assembly_source = 'elf.s'
endif

if host_machine.cpu_family() == 'x86_64'
    if get_option('avx')
        defs += '-mavx'
    endif
    if get_option('avx2')
        defs += [ '-mavx2', '-mfma' ]
    endif
    assembly_arch = 'amd64'
elif host_machine.cpu_family() == 'aarch64'
    assembly_arch = 'aarch64'
elif host_machine.cpu_family() == 'riscv64'
    assembly_arch = 'riscv'
    assembly_source = 'rv64gc.s'
else
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not supported.')
endif

engine_sources += files(
    'jump_fcontext_' + assembly_arch + '_' + assembly_source,
    'make_fcontext_' + assembly_arch + '_' + assembly_source,
)
