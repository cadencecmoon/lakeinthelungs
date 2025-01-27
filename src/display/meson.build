if sys == 'windows'
    #display_sources += files('')
    #
elif sys == 'darwin'
    #display_sources += files('')

    defs += [
        '-DPLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    add_languages('objc')
    deps += declare_dependency(link_args: [
        '-framework', 'Cocoa',
        '-framework', 'IOKit',
        '-framework', 'CoreFoundation' ])

    if host_machine.subsystem() == 'ios'
        #display_sources += files('ios.m')
        defs += '-DPLATFORM_IOS=1'
        deps += declare_dependency(link_args: [ '-framework', 'UIKit' ])
    else
        #display_sources += files('cocoa.m')
        defs += '-DPLATFORM_MACOS=1'
    endif

elif sys == 'android'
    #display_sources += files('')
    
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

elif sys == 'linux' or sys == 'bsd'
    dep_wayland_client = dependency('wayland-client', required : get_option('wayland'))
    dep_wayland_protocols = dependency('wayland-protocols', version : '>= 1.12', required : get_option('wayland'))
    dep_xkbcommon = dependency('xkbcommon', required : get_option('wayland'))
    wayland_scanner = find_program('wayland-scanner', required : get_option('wayland'))
    build_wayland = (dep_wayland_client.found() and dep_wayland_protocols.found() and
                        wayland_scanner.found() and dep_xkbcommon.found())
    if build_wayland
        display_sources += files('wayland.c')
        defs += [ 
            '-DHADOPELAGIC_WAYLAND=1', 
            '-DVK_USE_PLATFORM_WAYLAND_KHR' ]

        wayland_protocols_dir = '../../resources/wayland/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        wayland_protocols = [
            'wayland',
            'fractional-scale-v1',
            'viewporter',
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
                output : name + '-protocol-code.h',
                install : false)
            display_sources += [ client_header, private_code ]
            message('Generating Wayland \'' + name + '.xml\' protocol code.')
        endforeach
    endif

    dep_xcb = dependency('xcb', required : get_option('xcb'))
    dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))
    build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
    if build_xcb
        #display_sources += files()
        defs += [ 
            '-DHADOPELAGIC_XCB=1', 
            '-DVK_USE_PLATFORM_XCB_KHR' ]
    endif

    dep_udev = dependency('libudev', required : get_option('drm'))
    dep_drm = dependency('libdrm', required : get_option('drm'))
    dep_gbm = dependency('gbm', required : get_option('drm'))
    build_drm = dep_drm.found() and dep_gbm.found() and not get_option('drm').disabled()
    if build_drm
        #display_sources += files('')
        defs += [
            '-DHADOPELAGIC_DRM=1',
            '-DVK_USE_PLATFORM_KMS_KHR' ]
    endif
endif
