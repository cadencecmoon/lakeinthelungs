# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

if sys == 'android'
    #engine += files('')

    defs += [
        '-DAMW_PLATFORM_ANDROID=1',
        '-DVK_USE_PLATFORM_ANDROID_KHR' ]
    cross += [ '-u', 'ANativeActivity_onCreate' ]
    android_ndk = '/opt/android-ndk'

    dep_android = cc.find_library('android', required : true)
    dep_log = cc.find_library('log', required : true)

    # Native App Glue
    lib_android_native_glue = static_library('android_app_glue',
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        [android_ndk + '/sources/android/native_app_glue/android_native_app_glue.c'])

    dep_android_native_glue = declare_dependency(
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        link_with : lib_android_native_glue)

    deps += [ dep_android, dep_log, dep_android_native_glue ]
    summary_sys_apis += [ 'Android' ]
endif
