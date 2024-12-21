/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

layout (std140, row_major, binding = 0) uniform per_frame_constants {
    /** Bounding-box dependent constants needed for dequantization of vertex world-space positions. */
    vec3 g_mesh_dequantization_factor, g_mesh_dequantization_summand;
    /** The reciprocal of the minimal error that maps to a distinct color. */
    float g_error_factor;
    /** The transform from world space to projection space. */
    mat4 g_world_to_projection_space;
    /** The location of the camera in world space. */
    vec3 g_camera_position_world_space;
    /** The viewport size in pixels. */
    uvec2 g_viewport;
    /** The location of the mouse cursor in pixels, relative to the left top of the content area of the window */
    ivec2 g_cursor_position;
};
