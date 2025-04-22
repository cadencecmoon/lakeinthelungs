link_src = files(
    'linker.cpp',
)

libspirv_tools_link = library('spirv-tools-link',
    spirv_generated_headers,
    link_src,
    dependencies : [dep_spirv_tools, dep_spirv_tools_opt, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : not meson.is_subproject(),
    override_options : cross_options,
)

dep_spirv_tools_link = declare_dependency(
    link_with : libspirv_tools_link,
    include_directories : public_incdir,
    version : meson.project_version(),
)

libspirv_tools_link_native = library('spirv-tools-link-native',
    spirv_generated_headers,
    link_src,
    dependencies : [dep_spirv_tools_native, dep_spirv_tools_opt_native, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : false,
    native : true,
    override_options : native_options,
)

dep_spirv_tools_link_native = declare_dependency(
    link_with : libspirv_tools_link_native,
    include_directories : public_incdir,
    version : meson.project_version(),
)
