if sys == 'android' or sys == 'linux' or sys == 'bsd'
    sources += files(
        'linux_process.c',
    )
    defs += '-DLAKE_PLATFORM_LINUX=1'
    dep_udev = dependency('libudev', required : get_option('drmkms'))
    dep_drm = dependency('libdrm', required : get_option('drmkms'))
    dep_gbm = dependency('gbm', required : get_option('drmkms'))

    build_drm = dep_udev.found() and dep_drm.found() and dep_gbm.found() and not get_option('drmkms').disabled()
    if build_drm
        #sources += files('drmkms.c')
        defs += [ '-DHADAL_DRMKMS=1', '-DVK_USE_PLATFORM_KMS_KHR' ]
    endif
endif

if sys == 'linux' or sys == 'bsd'
    dep_xcb = dependency('xcb', required : get_option('xcb'))
    dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))

    build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
    if build_xcb
        #sources += files('xcb.c')
        defs += [ '-DHADAL_XCB=1', '-DVK_USE_PLATFORM_XCB_KHR' ]
    endif

    dep_wayland_client = dependency('wayland-client', required : get_option('wayland'))
    dep_xkbcommon = dependency('xkbcommon', required : get_option('wayland'))
    wayland_scanner = find_program('wayland-scanner', required : get_option('wayland'))

    build_wayland = (dep_wayland_client.found() and wayland_scanner.found() and dep_xkbcommon.found())
    if build_wayland
        sources += files('wayland.c')
        defs += [ '-DHADAL_WAYLAND=1', '-DVK_USE_PLATFORM_WAYLAND_KHR' ]

        wayland_protocols_dir = '../../wayland_protocols/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        if cc.has_header('libdecor.h', dependencies : [dep_rt])
            defs += '-DHADAL_WAYLAND_LIBDECOR=1'
        endif

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

            sources += [ client_header, private_code ]
            message('Generating Wayland \'' + name + '.xml\' protocol code.')
        endforeach
    endif

    dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
    if dep_pipewire.found()
        sources += files('pipewire.c')
        defs += '-DSOMA_PIPEWIRE=1'
    endif

    dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
    if dep_pulse.found()
        #sources += files('')
        defs += '-DSOMA_PULSEAUDIO=1'
    endif

    dep_alsa = dependency('alsa', required : get_option('alsa'))
    if dep_alsa.found()
        #sources += files('')
        defs += '-DSOMA_ALSA=1'
    endif

    found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
    if found_oss
        #sources += files()
        defs += '-DSOMA_OSS=1'
    endif
endif
