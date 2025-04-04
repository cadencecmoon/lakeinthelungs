dep_udev = dependency('libudev', required : get_option('drm'))
dep_drm = dependency('libdrm', required : get_option('drm'))
dep_gbm = dependency('gbm', required : get_option('drm'))

build_drm = dep_drm.found() and dep_gbm.found() and not get_option('drm').disabled()
if build_drm
    #engine_sources += files('drmkms.c')
    defs += [ '-DHADAL_DRMKMS=1', '-DVK_USE_PLATFORM_KMS_KHR' ]
endif
