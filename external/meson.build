# check if Vulkan SDK is available, we'll link at runtime
dep_vulkan = dependency('vulkan', required : get_option('vulkan'))
if dep_vulkan.found()
    sources += files('vk_mem_alloc.cpp')
endif

if get_option('imgui')
    sources += files(
        'imgui.cpp',
        'imgui_draw.cpp',
        'imgui_widgets.cpp',
        'imgui_tables.cpp',
    )
    defs += '-DLAKE_IMGUI=1'
    message('Building the engine with ImGUI editors enabled.')
endif
