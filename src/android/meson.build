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

