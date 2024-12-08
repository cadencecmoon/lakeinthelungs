# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

if sys == 'darwin'
    #engine = files('')

    defs += [
        '-DAMW_PLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    deps += declare_dependency(link_args: [
        '-framework', 'Cocoa',
        '-framework', 'IOKit',
        '-framework', 'CoreFoundation' ])
    add_languages('objc')

    if host_machine.subsystem() == 'ios'
        defs += '-DAMW_PLATFORM_IOS=1'
        deps += declare_dependency(link_args: [
            '-framework', 'UIKit' ])
        summary_sys_apis += [ 'Cocoa (iOS)' ]
    else
        defs += '-DAMW_PLATFORM_MACOS=1'
        symmary_sys_apis += [ 'Cocoa' ]
    endif
endif
