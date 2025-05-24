lake_sources += files(
    'bedrock.c',
    'utility.c',
    'math/bits.c',
    'data_structures/allocator.c',
    'data_structures/arena_allocator.c',
    'data_structures/bitset.c',
    'data_structures/block_allocator.c',
    'data_structures/darray.c',
    'data_structures/deque.c',
    'data_structures/hashmap.c',
    'data_structures/map.c',
    'data_structures/mpmc_ring.c',
    'data_structures/slab_allocator.c',
    'data_structures/sparse.c',
    'data_structures/stack_allocator.c',
    'data_structures/switch_list.c',
    'data_structures/tree_allocator.c',
)

lake_deps += dependency('threads', required: false)
lake_deps += cc.find_library('m', required: false)

# TODO move to ui?
with_imgui = get_option('imgui')
if with_imgui
    cimgui = subproject('cimgui')
    cimgui_dep = cimgui.get_variable('cimgui_dep')
    lake_deps += cimgui_dep
    pre_args += ['-DLAKE_IMGUI=1']
endif

# TODO move to audio?
ogg_dep = dependency('ogg', fallback : ['ogg', 'ogg_dep'])
vorbis_dep = dependency('vorbis', fallback : ['vorbis', 'vorbis_dep'])
if ogg_dep.found() and vorbis_dep.found()
    # XXX vorbis linked at runtime ?
    lake_deps += [ ogg_dep, vorbis_dep ]
    pre_args += '-DLAKE_VORBIS=1'
endif

subdir('asm')
subdir('unix')
subdir('linux')
