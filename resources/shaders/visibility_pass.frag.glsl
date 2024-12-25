/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#version 460

layout (location = 0) flat in uint g_primitve_index;

layout (location = 0) out uint g_out_color;

void main() {
    g_out_color = g_primitve_index;
}
