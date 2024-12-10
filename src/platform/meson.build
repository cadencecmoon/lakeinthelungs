# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

dep_threads = dependency('threads', required : true)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

engine += files(
    'dll.c',
    'hadal.c',
    'ticks.c',
)

subdir('android')
subdir('apple')
subdir('linux')
subdir('unix')
subdir('win32')
