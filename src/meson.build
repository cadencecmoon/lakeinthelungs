dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    engine_sources += files(
        'vk_backend.c',
        'vk_device.c',
        'vk_swapchain.c',
    )
    defs += '-DSILVERA_VULKAN=1'
endif

subdir('asm')
subdir('apple')
subdir('linux')
subdir('unix')
subdir('windows')

engine_sources += files(
    'bits.c',
    'frame_time.c',
    'hadal.c',
    'hash_table.c',
    'log.c',
    'octavia.c',
    'riven.c',
    'silvera.c',
    'string.c',
)
