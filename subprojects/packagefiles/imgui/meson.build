project('imgui', 'cpp', license : 'MIT', version : '1.91.9b', meson_version : '>=0.50.0')

include_dir = include_directories('.', 'backends')
sources = files(
    'imgui.cpp',
    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
)

api = '-DIMGUI_API=__attribute__((visibility("default")))'
if host_machine.system() == 'windows'
    api = '-DIMGUI_API=@0@'.format(
        get_option('default_library') != 'static' ? '__declspec(dllexport)' : '',
    )
endif

imgui = library(
    'imgui',
    sources,
    cpp_args : api,
    gnu_symbol_visibility : 'hidden',
    include_directories : include_dir,
    install : true)

if host_machine.system() == 'windows'
    api = '-DIMGUI_API=@0@'.format(
        get_option('default_library') != 'static' ? '__declspec(dllimport)' : '',
    )
endif

dep_imgui = declare_dependency(
    compile_args : api,
    include_directories : include_dir,
    link_with : imgui)
