/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include "spirv_shaders.h"

#include "visibility_pass.frag.glsl.spv.h"
#include "visibility_pass.vert.glsl.spv.h"
const size_t visibility_pass_vert_glsl_size = sizeof(visibility_pass_vert_glsl_spv);
const size_t visibility_pass_frag_glsl_size = sizeof(visibility_pass_frag_glsl_spv);

#include "lighting_pass.frag.glsl.spv.h"
#include "lighting_pass.vert.glsl.spv.h"
const size_t lighting_pass_vert_glsl_size = sizeof(lighting_pass_vert_glsl_spv);
const size_t lighting_pass_frag_glsl_size = sizeof(lighting_pass_frag_glsl_spv);
