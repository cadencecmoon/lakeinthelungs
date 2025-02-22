if sys == 'darwin'
    #engine_sources += files('')
    defs += [
        '-DPLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    add_languages('objc')
    deps += declare_dependency(link_args: [
        '-framework', 'Cocoa',
        '-framework', 'IOKit',
        '-framework', 'CoreFoundation' ])

    if host_machine.subsystem() == 'ios'
        defs += '-DPLATFORM_APPLE_IOS=1'
        deps += declare_dependency(link_args: [ '-framework', 'UIKit' ])
    else
        defs += '-DPLATFORM_APPLE_MACOS=1'
    endif

    dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('core'))
    if dep_core.found()
        defs += '-DOCTAVIA_COREAUDIO=1'
        deps += dep_core
    endif
endif
