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

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DAMW_DEBUG'
elif get_option('buildtype') == 'release'
    defs += '-DAMW_NDEBUG'
endif

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@'.format(sys))
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

subdir('shaders')
subdir('src')

lakeinthelungs = executable(
    'lakeinthelungs', 
    engine, shaders, main,
    c_args : defs,
    dependencies : deps,
    include_directories : [ 'src' ],
    install : false)

subdir('docs')
