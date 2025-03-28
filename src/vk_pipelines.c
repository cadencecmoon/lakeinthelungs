#include "vk_reznor.h"

FN_REZNOR_ASSEMBLY(vulkan, pipeline_layout) { (void)work; }
FN_REZNOR_DISASSEMBLY(vulkan, pipeline_layout) { (void)pipeline_layout; }

FN_REZNOR_ASSEMBLY(vulkan, graphics_pipeline) { (void)work; }
FN_REZNOR_DISASSEMBLY(vulkan, graphics_pipeline) { (void)graphics_pipeline; }

FN_REZNOR_ASSEMBLY(vulkan, compute_pipeline) { (void)work; }
FN_REZNOR_DISASSEMBLY(vulkan, compute_pipeline) { (void)compute_pipeline; }

FN_REZNOR_ASSEMBLY(vulkan, raytracing_pipeline) { (void)work; }
FN_REZNOR_DISASSEMBLY(vulkan, raytracing_pipeline) { (void)raytracing_pipeline; }

FN_REZNOR_ASSEMBLY(vulkan, shader_binding_table) { (void)work; }
FN_REZNOR_DISASSEMBLY(vulkan, shader_binding_table) { (void)shader_binding_table; }
