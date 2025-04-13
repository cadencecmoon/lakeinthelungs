project('lakeinthelungs', 'c',
    version : '2025',
    default_options : [
        'c_std=c99',
        'buildtype=release',
        'default_library=shared',
        'warning_level=3',
    ])

cc = meson.get_compiler('c')
sys = host_machine.system()
fs = import('fs')

defs = ['-Wall']
if cc.get_id() != 'msvc'
    defs += [
        '-Wno-pedantic',
        '-Werror=implicit-function-declaration',
        '-Werror=incompatible-pointer-types',
        '-Werror=return-type' ]
endif

deps = []
cross_opts = []
include = include_directories('include')

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DLAKE_DEBUG'
elif get_option('buildtype') == 'release'
    defs += '-DLAKE_NDEBUG'
endif
if get_option('default_library') == 'shared'
    defs += '-DLAKE_BUILD_DLL_EXPORT'
endif

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@').format(host_machine.system())
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

sources = []
subdir('src')

amwengine = library(
    'amwengine',
    sources,
    c_args : defs,
    dependencies : deps,
    include_directories : include,
    install : false)
dep_amwengine = declare_dependency(
    link_args : cross_opts,
    include_directories : include,
    link_with : amwengine)

main_sources = []
subdir('main')

lakeinthelungs = executable(
    'lakeinthelungs',
    main_sources,
    c_args : defs,
    dependencies : dep_amwengine,
    install : false)

doxygen = find_program('doxygen', required : get_option('docs'))
if doxygen.found()
    docs_cdata = configuration_data()
    docs_cdata.set('VERSION', meson.project_version())
    docs_cdata.set('TOP_SRCDIR', meson.project_source_root())
    docs_cdata.set('TOP_BUILDDIR', meson.project_source_root())

    doxyfile = configure_file(
        input : 'Doxyfile.in',
        output : 'Doxyfile',
        configuration : docs_cdata,
        install : false)
    docs_datadir = join_paths(get_option('datadir'), 'docs', 'lake')

    html_target = custom_target(
        'lakeinthelungs_docs',
        input : doxyfile,
        output : 'html',
        command : [ doxygen, doxyfile ],
        install : false, # TODO
        install_dir : docs_datadir)
endif

summary({
    'System' : systype,
    'Compiler' : cc.get_id(),
    'Linker' : cc.get_linker_id(),
}, bool_yn : true, section : 'Lake in the Lungs')
