# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

shaders_src = [
    'demo.frag.glsl',
    'demo.vert.glsl',
]

glslang = find_program('glslangValidator', required : get_option('vulkan'))

if glslang.found()
    foreach shader : shaders_src
        name = shader.underscorify() + '_spv'
        args = [ glslang, '-V', '@INPUT@', '-o', '@OUTPUT@', '--vn', name ]
        header = custom_target(
            shader + '_spv',
            output : shader + '.spv.h',
            input : shader,
            command : args,
            install : false)
        shaders += [ header ]
    endforeach
endif
