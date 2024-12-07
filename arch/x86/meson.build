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
    'context/jump_' + x86_config,
    'context/make_' + x86_config,
)
