assembly_dir = []
assembly_ext = '.s'

dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

if host_machine.cpu_family() == 'x86_64'
    # The baseline (x86_64-v1): MMX, SSE and SSE2 instructions, are implemented by all x86_64 CPUs
    
    if get_option('avx')
        defs += '-mavx'
    endif
    if get_option('avx2')
        defs += [ '-mavx2', '-mfma' ]
    endif

    # Resolve ABI of different supported platforms
    if sys == 'windows'
        assembly_dir = 'x86/amd64_pe/'
        assembly_ext = '.asm'
    elif sys == 'darwin'
        assembly_dir = 'x86/amd64_macho/'
    else
        assembly_dir = 'x86/amd64_elf/'
    endif
elif host_machine.cpu_family() == 'x86' 
    if sys == 'windows'
        assembly_dir = 'x86/i386_pe/'
        assembly_ext = '.asm'
    elif sys == 'darwin'
        assembly_dir = 'x86/i386_macho/'
    else
        assembly_dir = 'x86/i386_elf/'
    endif
elif host_machine.cpu_family() == 'arm'
    if sys == 'darwin'
        assembly_dir = 'arm/armv7_macho/'
    else
        assembly_dir = 'arm/armv7_elf/'
    endif
elif host_machine.cpu_family() == 'aarch64'
    if sys == 'windows'
        assembly_dir = 'arm/aarch64_pe/'
        assembly_ext = '.asm'
    elif sys == 'darwin'
        assembly_dir = 'arm/aarch64_macho/'
    else
        assembly_dir = 'arm/aarch64_elf/'
    endif
elif host_machine.cpu_family() == 'riscv32'
    assembly_dir = 'riscv/rv32gc/'
elif host_machine.cpu_family() == 'riscv64'
    assembly_dir = 'riscv/rv64gc/'
else
    # WebAssembly ?
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not yet supported.')
endif

subdir('linux')
subdir('posix')

bedrock_sources += files(
    'apptimer.c',
    'log.c',
    'riven.c',
    'util_mem.c',
    assembly_dir + 'jump_fcontext' + assembly_ext,
    assembly_dir + 'make_fcontext' + assembly_ext,
)
