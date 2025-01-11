/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#version 460
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_control_flow_attributes : enable

#include "shared_constants.glsl"

/** Bindings for mesh geometry. */
layout (binding = 1) uniform utextureBuffer g_quantized_vertex_positions;

/** The texture with primitive indices per pixel produced by the visibility pass. */
layout (binding = 2, input_attachment_index = 0) uniform usubpassInput g_visibility_buffer;

/** The pixel index with origin in the upper left corner. */
layout(origin_upper_left) in vec4 gl_FragCoord;

/** Color written to the swapchain image. */
layout (location = 0) out vec4 g_out_color;

void main() {
    /* set the background color */
    vec3 final_color = vec3(0.0f);

    /* TODO do the actual shading */

    /* if there are NaNs or INFs, we want to know - make them pink */
    if (isnan(final_color.r) || isnan(final_color.g) || isnan(final_color.b) ||
        isinf(final_color.r) || isinf(final_color.g) || isinf(final_color.b))
        final_color = vec3(1.0f, 0.0f, 0.8f);
    /* output the result of shading */
    g_out_color = vec4(final_color, 1.0f);
}
