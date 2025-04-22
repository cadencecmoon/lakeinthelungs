sources += files(
  'a_moonlit_walk.c',
  'gpuexec.c',
  'hadal.c',
  'rendering.c',
  'riven.c',
  'simulation.c',
  'soma.c',
  'xaku.c',
)

dep_threads = dependency('threads', required : true)
dep_dl = cc.find_library('dl', required : false)
dep_rt = cc.find_library('rt', required : false)

# vorbis linked at runtime ?
dep_vorbis = dependency('vorbis', fallback : ['vorbis', 'dep_vorbis'])
if dep_vorbis.found()
    defs += '-DLAKE_VORBIS=1'
endif

# enable imgui if requested developer tools
if enable_devtools
    sources += files(
        'devtools_imgui.cpp',
    )

    imgui = subproject('imgui')
    dep_imgui = imgui.get_variable('dep_imgui')

    deps += dep_imgui
    defs += '-DLAKE_IMGUI=1'
endif

# check if Vulkan SDK is available, we'll link to Vulkan at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
dep_vma = dependency('VulkanMemoryAllocator', 
    fallback : ['vulkan-memory-allocator', 'dep_vma'], 
    required : get_option('vulkan'))
vulkan_found = dep_vulkan.found() and dep_vma.found()
if vulkan_found
    sources += files(
        'vk_command_recorder.c',
        'vk_device.c',
        'vk_drivers.c',
        'vk_gpu_allocator.c',
        'vk_gpu_resources.c',
        'vk_mem_alloc.cpp',
        'vk_pipelines.c',
        'vk_swapchain.c',
        'vk_timeline_query.c',
    )
    deps += dep_vma # dont link vulkan
    defs += [ '-DXAKU_VULKAN=1', '-DXAKU_VULKAN_VMA=1' ]
endif

subdir('android')
subdir('apple')
subdir('bedrock')
subdir('linux')
subdir('unix')
subdir('windows')

