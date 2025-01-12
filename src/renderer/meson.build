# Lake in the Lungs
# Copyright (c) 2025 Cadence C. Moon
# The source code is licensed under a standard MIT license.

engine += files(
    'drivers.c',
    'pelagia.c',
    'pipelines.c',
    'spirv_shaders.c',
)

dep_vulkan = dependency('vulkan', required : true) #get_option('vulkan'))
defs += '-DAMW_NATIVE_VULKAN=1'
