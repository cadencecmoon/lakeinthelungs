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
cross_opts = []

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DDEBUG'
elif get_option('buildtype') == 'release'
    defs += '-DNDEBUG'
endif

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@').format(host_machine.system())
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

subdir('docs')
subdir('resources')

amw_sources = []
lake_sources = []
subdir('src')

a_moonlit_walk = library(
    'AMW', amw_sources,
    c_args : defs,
    dependencies : deps, 
    install : false)
dep_amw = declare_dependency(
    link_args : cross_opts,
    link_with : a_moonlit_walk)

lakeinthelungs = executable(
    'lakeinthelungs', 
    lake_sources,
    c_args : defs,
    dependencies : dep_amw,
    install : false)

summary({
    'System type' : systype,
    'C compiler' : cc.get_id(),
    'Linker' : cc.get_linker_id(),
}, bool_yn : true, section : 'Lake in the Lungs')
