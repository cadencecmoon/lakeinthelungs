moon_sources += files(
    'moon.c'
)

if not cpp.has_header('slang.h')
    slang = subproject('slang')
    moon_deps += slang.get_variable('slang_include_dep')
endif
pre_args += ['-DMOON_SLANG=1']

if with_vulkan
    subdir('vulkan')
endif
