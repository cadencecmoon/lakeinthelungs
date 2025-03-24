dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine_sources += files(
        'vk_device.c',
        'vk_swapchain.c',
        'vk_textures.c',
    )
    defs += '-DREZNOR_VULKAN=1'
endif

subdir('asm')
subdir('apple')
subdir('linux')
subdir('unix')
subdir('windows')

engine_sources += files(
    'a_moonlit_walk.c',
    'bits.c',
    'frame_time.c',
    'game.c',
    'gpuexec.c',
    'hadal.c',
    'hash_table.c',
    'log.c',
    'rendering.c',
    'reznor.c',
    'riven.c',
    'soma.c',
    'string.c',
)
