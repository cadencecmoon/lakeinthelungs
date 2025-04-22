dep_threads = dependency('threads', required : true)

subdir('asm')
subdir('audio')
subdir('bedrock')
subdir('display')
subdir('math')
subdir('renderer')

sources += files(
  'a_moonlit_walk.c',
  'gpuexec.c',
  'rendering.c',
  'riven.c',
  'simulation.c',
)

# enable imgui if requested developer tools
if enable_devtools
    imgui = subproject('imgui')
    dep_imgui = imgui.get_variable('dep_imgui')

    deps += dep_imgui
    defs += '-DLAKE_IMGUI=1'
endif
