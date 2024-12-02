# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

project('lakeinthelungs', 'c',
    version : '2024',
    default_options : [ 
        'c_std=c99',
        'buildtype=release',
        'default_library=static',
        'warning_level=3',
    ])

cc = meson.get_compiler('c')
sys = host_machine.system()
fs = import('fs')

engine = []
shaders = []
sources = []
include_arch = []

defs = [
    '-Wall',
    '-Wno-pedantic',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
    '-Werror=return-type', 
    '-fno-stack-protector' ]
linkarg = [ '-nostdlib' ] # scary !
deps = []

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DAMW_DEBUG'
    if cc.has_header('valgrind/memcheck.h')
        defs += '-DAMW_HAS_VALGRIND'
    endif
elif get_option('buildtype') == 'release'
    defs += '-DAMW_NDEBUG'
endif

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@').format(host_machine.system())
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

# For summary:
summary_sys_apis = []
summary_audio_apis = []
summary_render_apis = []

subdir('docs')
subdir('thirdparty')
subdir('assets')

subdir('arch')
subdir('audio')
subdir('core')
subdir('main')
subdir('physics')
subdir('renderer')
subdir('sys')

a_moonlit_walk = library('AMW',
    engine, shaders,
    c_args : defs,
    link_args : linkarg,
    dependencies : deps,
    include_directories : [ 'include', include_arch ],
    install : false)
dep_amw = declare_dependency(
    include_directories : [ 'include', include_arch ],
    link_args : linkarg,
    link_with : a_moonlit_walk)

lakeinthelungs = executable(
    'lakeinthelungs',
    sources,
    c_args : defs,
    dependencies : dep_amw,
    install : false)

summary({
    'System type' : systype,
    'Compiler' : cc.get_id(),
    'Linker' : cc.get_linker_id(),
    'Build type' : get_option('buildtype'),
}, bool_yn : true, section : 'A Moonlit Walk Engine')

summary({
    'Display APIs' : '@0@'.format(', '.join(summary_sys_apis)),
    'Audio APIs' : '@0@'.format(', '.join(summary_audio_apis)),
    'Rendering APIs' : '@0@'.format(', '.join(summary_render_apis)),
}, bool_yn : true, section : 'Optional Features')
