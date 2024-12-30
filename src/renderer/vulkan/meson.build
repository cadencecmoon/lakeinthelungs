dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine += files(
        'spirv_shaders.c',
        'vk_backend.c',
        'vk_device.c',
        'vk_drivers.c',
        'vk_surface.c',
    )
    defs += '-DAMW_NATIVE_VULKAN=1'
endif
