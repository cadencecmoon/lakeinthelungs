assembly_arch = []
assembly_os = []
assembly_abi = []
assembly_asm = []

if sys == 'windows'
    assembly_os = 'ms'
    assembly_abi = 'pe'
    assembly_asm = '.masm'
    add_languages('masm')
elif sys == 'darwin'
    assembly_os = 'sysv'
    assembly_abi = 'macho'
    assembly_asm = '.s'
else
    assembly_os = 'sysv'
    assembly_abi = 'elf'
    assembly_asm = '.s'
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
    assembly_os = 'aapcs'
    assembly_arch = 'aarch64'
elif host_machine.cpu_family() == 'riscv64'
    assembly_arch = 'riscv'
    assembly_abi = 'rv64gc'
else
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not supported.')
endif

assembly_config = '_' + assembly_arch + '_' + assembly_os + '_' + assembly_abi + assembly_asm
engine_sources += files(
    'jump_fcontext' + assembly_config,
    'make_fcontext' + assembly_config,
)
