lake_sources = files(
    'main.c',
    'simulation.c',
    'rendering.c',
    'gpuexec.c',
)

audio_sources = []
bedrock_sources = []
display_sources = []
math_sources = []
memory_sources = []
renderer_sources = []

subdir('audio')
subdir('bedrock')
subdir('display')
subdir('math')
subdir('memory')
subdir('renderer')

amw_sources += [
    audio_sources,
    bedrock_sources,
    display_sources,
    math_sources,
    memory_sources,
    renderer_sources,
]
