#pragma once

#include "amwe/graphics/xaku_device.h"

/** TODO move stuff from xaku pipelines that is relevant to compiling shaders and managing pipelines. */

/** Custom preprocessor for a pipeline manager. TODO filesystem path */
typedef void (LAKECALL *PFN_xaku_shader_custom_preprocessor)(const char *code, const char *path);

struct shader_pipeline_builder_assembly {
    lake_dynamic_array(const char *)                root_paths;                     /* TODO filesystem path */ 
    const char                                     *write_out_preprocessed_code;    /* TODO filesystem path */  
    const char                                     *write_out_spirv;                /* TODO filesystem path */  
    const char                                     *spirv_cache_folder;             /* TODO filesystem path */  
    PFN_xaku_shader_custom_preprocessor             custom_preprocessor;
    enum xaku_shader_language                       default_language;
    u32                                             default_create_flags; /**< enum xaku_shader_create_flag_bits */
    lake_dynamic_array(struct xaku_shader_define)   default_defines;
    u32                                             default_required_subgroup_size;
    bool                                            default_enable_debug_utils;
    bool                                            has_default_required_subgroup_size;
    bool                                            has_write_out_preprocessed_code;   
    bool                                            has_write_out_spirv;               
    bool                                            has_spirv_cache_folder;            
    bool                                            register_null_pipelines_when_first_compile_fails;
    lake_small_string                               name;
};
