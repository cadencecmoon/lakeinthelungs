dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

subdir('asm')
subdir('apple')
subdir('linux')
subdir('math')
subdir('unix')
subdir('vulkan')
subdir('windows')

engine_sources += files(
    'a_moonlit_walk.c',
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
