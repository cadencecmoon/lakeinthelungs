#include "vk_reznor.h"

void vulkan_pipeline_layout_destroy(struct reznor_pipeline_layout *restrict pipeline_layout)
{
    (void)pipeline_layout;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, pipeline_layout)
{
    (void)device;
    (void)pipeline_layout_count;
    (void)pipeline_layout_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

void vulkan_graphics_pipeline_destroy(struct reznor_graphics_pipeline *restrict pipeline)
{
    (void)pipeline;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, graphics_pipeline)
{
    (void)device;
    (void)graphics_pipeline_count;
    (void)graphics_pipeline_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

void vulkan_compute_pipeline_destroy(struct reznor_compute_pipeline *restrict pipeline)
{
    (void)pipeline;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, compute_pipeline)
{
    (void)device;
    (void)compute_pipeline_count;
    (void)compute_pipeline_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

void vulkan_raytracing_pipeline_destroy(struct reznor_raytracing_pipeline *restrict pipeline)
{
    (void)pipeline;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, raytracing_pipeline)
{
    (void)device;
    (void)raytracing_pipeline_count;
    (void)raytracing_pipeline_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

void vulkan_shader_binding_table_destroy(struct reznor_shader_binding_table *restrict shader_binding_table)
{
    (void)shader_binding_table;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, shader_binding_table)
{
    (void)device;
    (void)shader_binding_table_count;
    (void)shader_binding_table_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}
