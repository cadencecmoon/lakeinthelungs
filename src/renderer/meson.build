# Lake in the Lungs
# Copyright (c) 2025 Cadence C. Moon
# The source code is licensed under a standard MIT license.

engine += files(
    'pelagia.c',
    'mock.c',
)

dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine += files(
        'vulkan/vk_device.c',
        'vulkan/vk_drivers.c',
        'vulkan/vk_surface.c',
    )
    defs += '-DAMW_NATIVE_VULKAN=1'
endif
