dep_xcb = dependency('xcb', required : get_option('xcb'))
dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))

build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
if build_xcb
    #engine_sources += files('xcb.c')
    defs += [ '-DHADAL_XCB=1', '-DVK_USE_PLATFORM_XCB_KHR' ]
endif
