sources += files(
    'hadal.c',
    'null_display.c',
)

if have_posix
    dep_dl = cc.find_library('dl', required : true)
    dep_rt = cc.find_library('rt', required : false)

    defs += [ '-DLAKE_PLATFORM_UNIX=1', '-D_GNU_SOURCE']
    deps += [ dep_threads, dep_rt, dep_dl ]

    if cc.has_function('clock_gettime', dependencies : [dep_rt])
        defs += '-DLAKE_HAS_CLOCK_GETTIME=1'
    endif
endif

if sys == 'android'
    #sources += files('android.c')
    defs += [ '-DLAKE_PLATFORM_ANDROID=1', '-DVK_USE_PLATFORM_ANDROID_KHR' ]
    cross_opts += [ '-u', 'ANativeActivity_onCreate' ]

    # TODO meson.get_external_property('android_ndk', '')
    # if android_ndk != ''
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
endif

if sys == 'android' or sys == 'linux' or sys == 'bsd'
    defs += '-DLAKE_PLATFORM_LINUX=1'
    dep_udev = dependency('libudev', required : get_option('drmkms'))
    dep_drm = dependency('libdrm', required : get_option('drmkms'))
    dep_gbm = dependency('gbm', required : get_option('drmkms'))

    build_drm = dep_udev.found() and dep_drm.found() and dep_gbm.found() and not get_option('drmkms').disabled()
    if build_drm
        #sources += files('drmkms.c')
        defs += [ '-DHADAL_DRMKMS=1', '-DVK_USE_PLATFORM_KMS_KHR' ]
    endif
elif sys == 'emscripten'
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

elif sys == 'windows'
    #sources += files('win32.c')
    defs += [
        '-DLAKE_PLATFORM_WINDOWS=1',
        '-DUNICODE',
        '-D_WIN32_WINNT=0x0A00',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]

elif sys == 'darwin'
    defs += [
        '-DLAKE_PLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    add_languages('objc')

    iphone_check = '''#include <TargetConditionals.h>
    #if !TARGET_OS_IPHONE
    #error not iphone
    #endif'''

    if cc.compiles(iphone_check, name: 'iPhone target')
        #sources += files('uikit.c')
        defs += '-DLAKE_PLATFORM_APPLE_IOS=1'
        deps += declare_dependency(link_args: [ '-framework', 'UIKit' ])
    else
        #sources += files('cocoa.c')
        defs += '-DLAKE_PLATFORM_APPLE_MACOS=1'
    endif
endif
