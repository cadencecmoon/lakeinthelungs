# Lake in the Lungs
# Copyright (c) 2025 Cadence C. Moon
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

defs = [
  '-Wall',
  '-Wno-pedantic',
  '-Werror=implicit-function-declaration',
  '-Werror=incompatible-pointer-types',
  '-Werror=return-type' ]
deps = []

engine = []
shaders = [] 
main = []
cross = []
include = include_directories('include')

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DAMW_DEBUG'
elif get_option('buildtype') == 'release'
    defs += '-DAMW_NDEBUG'
endif

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@').format(host_machine.system())
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

subdir('docs')
subdir('tools')
subdir('src')

a_moonlit_walk = library(
    'AMW', engine, shaders,
    c_args : defs,
    dependencies : deps,
    include_directories : include,
    install : false)
dep_amw = declare_dependency(
    include_directories : include,
    link_args : cross,
    link_with : a_moonlit_walk)

subdir('main')

lakeinthelungs = executable(
    'lakeinthelungs', main,
    c_args : defs,
    dependencies : dep_amw,
    include_directories : 'main',
    install : false)

summary({
    'System type' : systype,
    'Compiler' : cc.get_id(),
    'Linker' : cc.get_linker_id(),
    'Build type' : get_option('buildtype'),
}, bool_yn : true, section : 'A Moonlit Walk Engine')
