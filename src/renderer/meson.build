engine += files(
    'cobalt.c',
    'mock.c',
)

dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine += files(
        'vulkan/spirv_shaders.c',
        'vulkan/vk_device.c',
        'vulkan/vk_drivers.c',
        'vulkan/vk_surface.c',
    )
    defs += '-DAMW_NATIVE_VULKAN=1'
endif
