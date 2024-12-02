# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

include_arch += include_directories('include')

# enable AVX instructions 
if get_option('x86_avx')
    defs += '-mavx'
endif

if cpufamily == 'x86'
    x86_cpu = 'i386'
else
    x86_cpu = 'amd64'
endif

if sys == 'windows'
    x86_abi = 'pe'
    x86_asm = '.asm'
elif sys == 'darwin'
    x86_abi = 'macho'
    x86_asm = '.s'
else
    x86_abi = 'elf'
    x86_asm = '.s'
endif

x86_config = x86_cpu + '_' + x86_abi + x86_asm
engine += files(
    'main/start_' + x86_config,
    # 'context/jump_' + x86_config,
    # 'context/make_' + x86_config,
)

if sys == 'linux' or sys == 'android' or sys == 'bsd'
    if cpufamily == 'x86'
        systable_arch = '32'
    else
        systable_arch = '64'
    endif
    systable_tbl = meson.current_source_dir() + '/syscall/syscall_' + systable_arch + '.tbl'
    systable_sh = meson.current_source_dir() + '/../../tools/scripts/systable_' + systable_arch + '.sh'

    systable = custom_target('posix_syscalls_table.h',
        input : systable_tbl,
        output : 'posix_syscalls_table.h',
        command : [ systable_sh, '@INPUT@', '@OUTPUT@' ],
        install : false)

    engine += systable
    engine += files(
        'syscall/syscall_' + x86_config,
    )
endif
