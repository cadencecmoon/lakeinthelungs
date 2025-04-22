if sys == 'darwin'
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

    dep_metal = dependency('appleframeworks', modules : [ 'Metal' ], required : get_option('metal'))
    if dep_metal.found()
        #sources += files()
        defs += '-DXAKU_METAL=1'
    endif

    dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('coreaudio'))
    if dep_core.found()
        #sources += files('')
        defs += '-DSOMA_COREAUDIO=1'
        deps += dep_core
    endif
endif
