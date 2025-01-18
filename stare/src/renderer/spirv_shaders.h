/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_VULKAN_SPIRV_SHADERS_H
#define _AMW_VULKAN_SPIRV_SHADERS_H

#include <stdint.h>
#include <stddef.h>

/* visibility pass */
extern const uint32_t visibility_pass_vert_glsl_spv[];
extern const uint32_t visibility_pass_frag_glsl_spv[];

/* lighting pass */
extern const uint32_t lighting_pass_vert_glsl_spv[];
extern const uint32_t lighting_pass_frag_glsl_spv[];

#endif /* _AMW_VULKAN_SPIRV_SHADERS_H */
