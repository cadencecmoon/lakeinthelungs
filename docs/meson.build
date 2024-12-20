# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

if get_option('docs')
    doxygen = find_program('doxygen', required : get_option('docs'))
    if doxygen.found()

        docs_cdata = configuration_data()
        docs_cdata.set('VERSION', meson.project_version())
        docs_cdata.set('TOP_SRCDIR', meson.project_source_root())
        docs_cdata.set('TOP_BUILDDIR', meson.project_build_root())

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
            install : true,
            install_dir : docs_datadir)
    endif
endif
