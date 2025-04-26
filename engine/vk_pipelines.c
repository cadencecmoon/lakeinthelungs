#include "vk_xaku.h"
#ifdef XAKU_VULKAN

FN_XAKU_COMPUTE_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_compute_pipeline;
    return lake_result_max_enum;
}

FN_XAKU_COMPUTE_PIPELINE_DISASSEMBLY(vulkan)
{
    (void)compute_pipeline;
}

FN_XAKU_RASTER_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_raster_pipeline;
    return lake_result_max_enum;
}

FN_XAKU_RASTER_PIPELINE_DISASSEMBLY(vulkan)
{
    (void)raster_pipeline;
}

FN_XAKU_RAY_TRACING_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_ray_tracing_pipeline;
    return lake_result_max_enum;
}

FN_XAKU_RAY_TRACING_PIPELINE_DISASSEMBLY(vulkan)
{
    (void)ray_tracing_pipeline;
}

#endif /* XAKU_VULKAN */
