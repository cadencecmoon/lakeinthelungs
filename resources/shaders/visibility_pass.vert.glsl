/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#version 460
#extension GL_GOOGLE_include_directive : enable
#include "shared_constants.glsl"
#include "mesh_quantization.glsl"

/** The quantized world space position from the vertex buffer. */
layout (location = 0) in uvec2 g_quantized_vertex_position;

layout (location = 0) flat out uint g_out_primitve_index;

void main() {
    vec3 vertex_position_world_space = dequantize_position_64_bit(g_quantized_vertex_position,
        g_mesh_dequantization_factor, g_mesh_dequantization_summand);
    gl_Position = g_world_to_projection_space * vec4(vertex_position_world_space, 1.0f);
    /** Without index buffer, the primitive index is the vertex index divided by three. */
    g_out_primitve_index = gl_VertexIndex / 3;
}
