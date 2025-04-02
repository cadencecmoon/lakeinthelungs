# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine_sources += files(
        'vk_buffers.c',
        'vk_commands.c',
        'vk_descriptor_sets.c',
        'vk_device.c',
        'vk_drivers.c',
        'vk_gpu_allocator.c',
        'vk_gpu_profiler.c',
        'vk_pipelines.c',
        'vk_swapchain.c',
        'vk_synchronization.c',
        'vk_textures.c',
    )
    defs += '-DREZNOR_VULKAN=1'
endif
