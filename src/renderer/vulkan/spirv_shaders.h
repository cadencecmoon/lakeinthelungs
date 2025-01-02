#ifndef _AMW_COBALT_VULKAN_SPIRV_SHADERS_H
#define _AMW_COBALT_VULKAN_SPIRV_SHADERS_H

#include <stdint.h>
#include <stddef.h>

/* Deferred: Visibility Pass */
extern const uint32_t visibility_pass_vert_glsl_spv[];
extern const size_t   visibility_pass_vert_glsl_size;

extern const uint32_t visibility_pass_frag_glsl_spv[];
extern const size_t   visibility_pass_frag_glsl_size;

/* Deferred: Shading Pass */
extern const uint32_t shading_pass_vert_glsl_spv[];
extern const size_t   shading_pass_vert_glsl_size;

extern const uint32_t shading_pass_frag_glsl_spv[];
extern const size_t   shading_pass_frag_glsl_size;

#endif /* _AMW_COBALT_VULKAN_SPIRV_SHADERS_H */
