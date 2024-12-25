shaders_src = [
    'visibility_pass.frag.glsl',
    'visibility_pass.vert.glsl',
    'shading_pass.frag.glsl',
    'shading_pass.vert.glsl',
]

glslang = find_program('glslangValidator', required : true)

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
