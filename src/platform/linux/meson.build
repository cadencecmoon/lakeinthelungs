if sys == 'linux' or sys == 'bsd'
    # Wayland
    dep_wayland_client = dependency('wayland-client', required : get_option('wayland'))
    dep_wayland_protocols = dependency('wayland-protocols', version : '>= 1.12', required : get_option('wayland'))
    dep_xkbcommon = dependency('xkbcommon', required : get_option('wayland'))
    wayland_scanner = find_program('wayland-scanner', required : get_option('wayland'))
    build_wayland = (dep_wayland_client.found() and dep_wayland_protocols.found() and
                        wayland_scanner.found() and dep_xkbcommon.found())
    if build_wayland
        engine += files(
            'wl_display.c',
            'wl_monitor.c',
            'wl_window.c',
        )

        defs += [ 
            '-DAMW_NATIVE_WAYLAND=1', 
            '-DVK_USE_PLATFORM_WAYLAND_KHR' ]

        wayland_protocols_dir = '../../../resources/wayland/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        wayland_protocols = [
            'wayland',
            'fractional-scale-v1',
            'idle-inhibit-unstable-v1',
            'pointer-constraints-unstable-v1',
            'pointer-gestures-unstable-v1',
            'relative-pointer-unstable-v1',
            'tablet-v2',
            'viewporter',
            'xdg-activation-v1',
            'xdg-decoration-unstable-v1',
            'xdg-shell',
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
                output : name + '-protocol-code.c',
                install : false)
            engine += [ client_header, private_code ]
            message('Generating Wayland \'' + name + '.xml\' protocol code.')
        endforeach
    endif

    # X11 (xcb)
    dep_xcb = dependency('xcb', required : get_option('xcb'))
    dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))
    build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
    if build_xcb
        #engine += files()

        defs += [ 
            '-DAMW_NATIVE_XCB=1', 
            '-DVK_USE_PLATFORM_XCB_KHR' ]
    endif
endif

# Linux (DRM/KMS)
if sys == 'android' or sys == 'linux' or sys == 'bsd'
    engine += files(
        'linux_system.c',
    )

    defs += '-DAMW_PLATFORM_LINUX=1'

    # DRM/KMS
    dep_udev = dependency('libudev', required : get_option('kms'))
    dep_drm = dependency('libdrm', required : get_option('kms'))
    dep_gbm = dependency('gbm', required : get_option('kms'))
    build_drmkms = dep_drm.found() and dep_gbm.found() and not get_option('kms').disabled()
    if build_drmkms
        #engine += files('')

        defs += [
            '-DAMW_NATIVE_DRM=1',
            '-DVK_USE_PLATFORM_KMS_KHR' ]
    endif
endif
