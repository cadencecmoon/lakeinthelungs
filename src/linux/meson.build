if sys == 'android'
    #engine_sources += files('')
    defs += [
        '-DPLATFORM_ANDROID=1',
        '-DVK_USE_PLATFORM_ANDROID_KHR' ]
    cross_opts += [ '-u', 'ANativeActivity_onCreate' ]

    android_ndk = '/opt/android-ndk'
    dep_android = cc.find_library('android', required : true)
    dep_log = cc.find_library('log', required : true)

    lib_android_native_glue = static_library('android_app_glue',
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        [android_ndk + '/sources/android/native_app_glue/android_native_app_glue.c'])
    dep_android_native_glue = declare_dependency(
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        link_with : lib_android_native_glue)
    deps += [ dep_android, dep_log, dep_android_native_glue ]

    dep_aaudio = cc.find_library('aaudio', required : get_option('aaudio'))
    if dep_aaudio.found()
        #engine_sources += files()
        defs += '-DSOMA_AAUDIO=1'
        deps += dep_aaudio
    endif
endif

if sys == 'android' or sys == 'linux' or sys == 'bsd'
    engine_sources += files(
        'process.c',
    )
    defs += '-DPLATFORM_LINUX=1'

    dep_udev = dependency('libudev', required : get_option('drm'))
    dep_drm = dependency('libdrm', required : get_option('drm'))
    dep_gbm = dependency('gbm', required : get_option('drm'))
    build_drm = dep_drm.found() and dep_gbm.found() and not get_option('drm').disabled()
    if build_drm
        #engine_sources += files('drmkms.c')
        defs += [ '-DHADAL_DRMKMS=1', '-DVK_USE_PLATFORM_KMS_KHR' ]
    endif
endif

if sys == 'linux' or sys == 'bsd'
    dep_xcb = dependency('xcb', required : get_option('xcb'))
    dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))
    build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
    if build_xcb
        #engine_sources += files('xcb.c')
        defs += [ '-DHADAL_XCB=1', '-DVK_USE_PLATFORM_XCB_KHR' ]
    endif

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

        wayland_protocols_dir = '../../wayland_protocols/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        wayland_protocols = [
            'cursor-shape-v1',
            'fractional-scale-v1',
            'idle-inhibit-unstable-v1',
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

    dep_alsa = dependency('alsa', required : get_option('alsa'))
    if dep_alsa.found()
        #engine_sources += files('')
        defs += '-DSOMA_ALSA=1'
    endif

    dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
    if dep_pulse.found()
        #engine_sources += files('')
        defs += '-DSOMA_PULSEAUDIO=1'
    endif

    dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
    if dep_pipewire.found()
        engine_sources += files('pipewire.c')
        defs += '-DSOMA_PIPEWIRE=1'
    endif
endif
