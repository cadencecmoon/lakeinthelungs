/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#version 460

/** The projection space position comes directly from the vertex buffer. */
layout (location = 0) in ivec2 g_projection_space_position;

void main() {
    vec2 position_float = vec2(g_projection_space_position);
    gl_Position = vec4(position_float, 0.0f, 1.0f);
}
