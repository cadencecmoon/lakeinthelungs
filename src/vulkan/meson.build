# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine_sources += files(
        'vk_device.c',
        'vk_pelagia.c',
        'vk_swapchain.c',
    )
    defs += '-DPELAGIA_VULKAN=1'
endif
