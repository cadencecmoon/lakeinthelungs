#include "vk_reznor.h"

void vulkan_pipeline_layout_destroy(struct reznor_pipeline_layout *restrict pipeline_layout)
{
    (void)pipeline_layout;
}

void vulkan_graphics_pipeline_destroy(struct reznor_graphics_pipeline *restrict pipeline)
{
    (void)pipeline;
}

void vulkan_compute_pipeline_destroy(struct reznor_compute_pipeline *restrict pipeline)
{
    (void)pipeline;
}

void vulkan_raytracing_pipeline_destroy(struct reznor_raytracing_pipeline *restrict pipeline)
{
    (void)pipeline;
}

void vulkan_shader_binding_table_destroy(struct reznor_shader_binding_table *restrict shader_binding_table)
{
    (void)shader_binding_table;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, pipeline_layout)
{
    (void)work;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, graphics_pipeline)
{
    (void)work;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, compute_pipeline)
{
    (void)work;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, raytracing_pipeline)
{
    (void)work;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, shader_binding_table)
{
    (void)work;
}
