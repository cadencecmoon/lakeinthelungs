dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine += files(
        'spirv_shaders.c',
        'vk_drivers.c',
    )
    defs += '-DAMW_NATIVE_VULKAN=1'
endif
