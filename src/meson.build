dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

subdir('asm')
subdir('apple')
subdir('linux')
subdir('unix')
subdir('vulkan')
subdir('windows')

engine_sources += files(
    'bits.c',
    'frame_time.c',
    'hadal.c',
    'hash_table.c',
    'log.c',
    'octavia.c',
    'pelagia.c',
    'riven.c',
    'string.c',
)
