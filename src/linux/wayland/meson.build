dep_wayland_client = dependency('wayland-client', required : get_option('wayland'))
dep_xkbcommon = dependency('xkbcommon', required : get_option('wayland'))
wayland_scanner = find_program('wayland-scanner', required : get_option('wayland'))

build_wayland = (dep_wayland_client.found() and wayland_scanner.found() and dep_xkbcommon.found())
if build_wayland
    engine_sources += files(
        'wl_display.c',
        'wl_window.c',
    )
    defs += [ '-DHADAL_WAYLAND=1', '-DVK_USE_PLATFORM_WAYLAND_KHR' ]

    wayland_protocols_dir = '../../../wayland_protocols/'
    client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
    private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

    wayland_protocols = [
        'alpha-modifier-v1',
        'color-management-v1',
        'cursor-shape-v1',
        'fractional-scale-v1',
        'frog-color-management-v1',
        'idle-inhibit-unstable-v1',
        'input-timestamps-unstable-v1',
        'keyboard-shortcuts-inhibit-unstable-v1',
        'pointer-constraints-unstable-v1',
        'pointer-gestures-unstable-v1',
        'primary-selection-unstable-v1',
        'relative-pointer-unstable-v1',
        'tablet-unstable-v2',
        'text-input-unstable-v3',
        'viewporter',
        'wayland',
        'xdg-activation-v1',
        'xdg-decoration-unstable-v1',
        'xdg-dialog-v1',
        'xdg-foreign-unstable-v2',
        'xdg-output-unstable-v1',
        'xdg-shell',
        'xdg-toplevel-icon-v1',
    ]

    foreach name : wayland_protocols
        protocol_xml_path = wayland_protocols_dir + '@0@.xml'.format(name)

        client_header = custom_target(
            ['@0@ client-header'.format(name)],
            command : client_header_args,
            input : protocol_xml_path,
            output : name + '-protocol.h',
            install : false)

        private_code = custom_target(
            ['@0@ private-code'.format(name)],
            command : private_code_args,
            input : protocol_xml_path,
            output : name + '-protocol-code.h',
            install : false)

        engine_sources += [ client_header, private_code ]
        message('Generating Wayland \'' + name + '.xml\' protocol code.')
    endforeach
endif
