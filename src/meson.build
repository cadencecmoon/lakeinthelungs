dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : true)
defs += '-DHARRIDAN_VULKAN=1'

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
    assembly_arch + '/jump_fcontext_' + assembly_source,
    assembly_arch + '/make_fcontext_' + assembly_source,
    'bits.c',
    'frame_time.c',
    'hadal.c',
    'harridan.c',
    'hash_table.c',
    'log.c',
    'riven.c',
    'vk_drivers.c',
)
