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

dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    sources += files(
        'vulkan/vk_device.c',
        'vulkan/vk_encore.c',
    )
    defs += '-DXAKU_VULKAN=1'
endif
