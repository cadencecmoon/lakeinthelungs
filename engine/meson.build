sources += files(
    'a_moonlit_walk.c',
    'frame_time.c',
    'hadal.c',
    'hash_table.c',
    'logger.c',
    'mem_utility.c',
    'riven.c',
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
    sources += files('imgui_dev.cpp')

    imgui = subproject('imgui')
    dep_imgui = imgui.get_variable('dep_imgui')

    deps += dep_imgui
    defs += '-DLAKE_IMGUI=1'
endif

# connect with the slang compiler
dep_slang = dependency('slang', fallback : ['slang', 'dep_slang'])
if dep_slang.found()
    defs += '-DLAKE_SLANG=1'
endif

# check if Vulkan SDK is available, we'll link to Vulkan at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
dep_vma = dependency('VulkanMemoryAllocator', 
    fallback : ['vulkan-memory-allocator', 'dep_vma'], 
    required : get_option('vulkan'))
vulkan_found = dep_vulkan.found() and dep_vma.found()
if vulkan_found
    sources += files(
        'vk_backend.c',
        'vk_command_recorder.c',
        'vk_device.c',
        'vk_mem_alloc.cpp',
        'vk_pipelines.c',
        'vk_swapchain.c',
        'vk_timeline_query.c',
    )
    deps += dep_vma # dont link vulkan
    defs += [ '-DXAKU_VULKAN=1', '-DXAKU_VULKAN_VMA=1' ]
endif

subdir('asm')
subdir('android')
subdir('apple')
subdir('linux')
subdir('math')
subdir('unix')
subdir('windows')
