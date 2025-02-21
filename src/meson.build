dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

subdir('apple')
subdir('linux')
subdir('unix')
subdir('windows')

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
    assembly_arch = 'amd64'
elif host_machine.cpu_family() == 'aarch64'
    assembly_arch = 'aarch64'
elif host_machine.cpu_family() == 'riscv64'
    assembly_arch = 'riscv'
    assembly_source = 'rv64gc.s'
else
    # WebAssembly? i386? arm32? rv32gc?
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not supported.')
endif

engine_sources += files(
    assembly_arch + '/jump_fcontext_' + assembly_source,
    assembly_arch + '/make_fcontext_' + assembly_source,
    'bits.c',
    'frame_time.c',
    'hadal.c',
    'hash_table.c',
    'log.c',
    'riven.c',
)
