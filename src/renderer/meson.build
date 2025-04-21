sources += files(
    'xaku.c'
)

dep_d3d12 = cc.find_library('d3d12', required : get_option('d3d12'))
dep_dxcore = cc.find_library('dxcore', required : cc.get_id() == 'msvc')
if dep_d3d12.found()
    #sources += files('')
    defs += '-DXAKU_D3D12=1'
endif

dep_metal = dependency('appleframeworks', modules : [ 'Metal' ], required : get_option('metal'))
if dep_metal.found()
    #sources += files()
    defs += '-DXAKU_METAL=1'
endif

webgpu_found = (sys == 'emscripten')
if (webgpu_found)
    #sources += files()
    defs += '-DXAKU_WEBGPU=1'
endif

# at external/
if dep_vulkan.found()
    sources += files(
        'vk_commands.c',
        'vk_device.c',
        'vk_encore.c',
        'vk_gpu_allocator.c',
        'vk_gpu_profiler.c',
        'vk_gpu_resources.c',
        'vk_pipelines.c',
        'vk_swapchain.c',
    )
    defs += '-DXAKU_VULKAN=1'
endif
