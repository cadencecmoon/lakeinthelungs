#include "vk_xaku.h"
#ifdef XAKU_VULKAN

FN_XAKU_COMMAND_RECORDER_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_command_recorder;
    return lake_result_max_enum;
}

FN_XAKU_COMMAND_RECORDER_DISASSEMBLY(vulkan)
{
    (void)command_recorder;
}

FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_CLEAR_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_CLEAR_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_BUFFER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)buffer;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_TEXTURE_DEFERRED(vulkan)
{
    (void)cmd;
    (void)texture;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan)
{
    (void)cmd;
    (void)texture_view;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_SAMPLER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)sampler;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_TLAS_DEFERRED(vulkan)
{
    (void)cmd;
    (void)tlas;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DESTROY_BLAS_DEFERRED(vulkan)
{
    (void)cmd;
    (void)blas;
    return lake_result_max_enum;
}

FN_XAKU_CMD_PUSH_CONSTANT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_SET_COMPUTE_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return lake_result_max_enum;
}

FN_XAKU_CMD_SET_RASTER_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return lake_result_max_enum;
}

FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return lake_result_max_enum;
}

FN_XAKU_CMD_SET_VIEWPORT(vulkan)
{
    (void)cmd;
    (void)viewport;
}

FN_XAKU_CMD_SET_SCISSOR(vulkan)
{
    (void)cmd;
    (void)scissor;
}

FN_XAKU_CMD_SET_DEPTH_BIAS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_SET_INDEX_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_BEGIN_RENDERING(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_END_RENDERING(vulkan)
{
    (void)cmd;
}

FN_XAKU_CMD_DISPATCH(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DISPATCH_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DRAW(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_DRAW_INDEXED(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_DRAW_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_DRAW_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_MESH_TASKS(vulkan)
{
    (void)cmd;
    (void)x;
    (void)y;
    (void)z;
}

FN_XAKU_CMD_MESH_TASKS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_MESH_TASKS_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_TRACE_RAYS(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_TRACE_RAYS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
    return lake_result_max_enum;
}

FN_XAKU_CMD_WRITE_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_RESET_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_BEGIN_LABEL(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_XAKU_CMD_END_LABEL(vulkan)
{
    (void)cmd;
}

FN_XAKU_COMPILE_COMMAND_LIST(vulkan)
{
    (void)cmd;
    (void)out_device_index;
    (void)out_list;
    return lake_result_max_enum;
}

#endif /* XAKU_VULKAN */
