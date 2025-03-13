#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>
#include <amw/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PELAGIA_MAX_FRAMES_IN_FLIGHT
#define PELAGIA_MAX_FRAMES_IN_FLIGHT 4
#endif
#ifndef PELAGIA_MAX_SWAPCHAIN_IMAGES
#define PELAGIA_MAX_SWAPCHAIN_IMAGES 8
#endif

#ifndef PELAGIA_ENABLE_GPU_PROFILER
    #ifndef NDEBUG
        #define PELAGIA_ENABLE_GPU_PROFILER 1
    #else
        #define PELAGIA_ENABLE_GPU_PROFILER 0
    #endif
#endif

typedef u32 render_resource_t;

/** The rendering device is our context of execution, used to create resources and run commands 
 *  on the GPU. (struct pelagia_device *) can be safely cast into (struct pelagia_interface **). */
struct pelagia_device;
/** A handle to memory allocated on the GPUs VRAM. Used to control how allocating GPU resources is done. */
struct pelagia_device_memory;
/** A buffer handle. Buffers hold data for different purposes, e.g. vertex data, index data, uniform data, etc. */
struct pelagia_buffer;
/** A texture handle. Textures represent 2D or 3D images, with properties like color, depth and stencil. */
struct pelagia_texture;
/** A sampler handle. Samplers control how textures are fetched, e.g. filters, wrapping. */
struct pelagia_sampler;
/** A shader module handle. It's a representation of a compiled shader, separate from the shader pipeline. */
struct pelagia_shader;
/** A pipeline layout handle. Defines the layout of a pipeline, including information on descriptor sets, push constants, etc. */
struct pelagia_pipeline_layout;
/** A graphics pipeline handle. Defines the configuration of a pipeline for rendering work. */
struct pelagia_graphics_pipeline;
/** A compute pipeline handle. Defines the configuration of a pipeline for compute work. */
struct pelagia_compute_pipeline;
/** A raytracing pipeline handle. Defines the configuration of a specialized pipeline for ray tracing operations. */
struct pelagia_raytracing_pipeline;
/** A shader binding table handle. SBT's are used in raytracing pipelines to specify which shaders should be 
 *  invoked for different rays. For example, in Vulkan they are GPU memory buffers storing references to shader 
 *  groups, used when calling e.g. 'vkCmdTraceRaysKHR()'. They allow to define custom raytracing behaviour. */
struct pelagia_shader_binding_table;
/** A command buffer handle. They hold a sequence of commands for the GPU to execute. 
 *  Pelagia is designed in a way to exploit parallel recording and submition of GPU commands. */
struct pelagia_command_buffer;
/** A descriptor set handle. They group descriptors (uniforms, textures, etc.) for use by shaders. */
struct pelagia_descriptor_set;
/** A query pool handle. Query pools are used to gather GPU execution statistics, like timestamps, occlusion queries, 
 *  or pipeline statistics - counting how many vertices, primitves, or fragments were processed, etc. They are used 
 *  for profiling, visibility queries and conditional rendering. */
struct pelagia_query_pool;
/** A swapchain handle. The swapchain is a way to present rendered images into a window surface.
 *  We must interface with Hadal to create a surface that Pelagia can draw to. */
struct pelagia_swapchain;
/** Acceleration structures are spatial data structures, typically implemented as bounding volume hierarchies (BVHs)
 *  that efficiently organize scene geometry, to minimize the number of ray-primitive intersection tests. A Bottom-level
 *  acceleration structure (BLAS) organizes geometry of a single mesh (BLAS is a 'raytracing mesh'). */
struct pelagia_bottom_level;
/** Top-level acceleration structures (TLAS) reference instances of bottom-level acceleration structures, for organizing
 *  instances of these meshes into a scene hierarchy (TLAS is a 'raytracing scene'). */
struct pelagia_top_level;
/** An opaque handle of the rendering backend. Pointer can be safely cast into 
 *  (struct pelagia_interface *) if obtained from a valid renderer implementation. */
struct pelagia;

/** Information needed to construct a rendering backend. */
struct pelagia_create_info {
    struct riven_create_info_header header;
    b32 debug_utilities;
};

/** Optional GPU features of the renderer, may be different per-device. */
enum pelagia_features {
    pelagia_feature_swapchain                   = (1u << 0),
    pelagia_feature_sparse_binding              = (1u << 1),
    pelagia_feature_acceleration_structures     = (1u << 2),
    pelagia_feature_raytracing_pipeline         = (1u << 3),
    pelagia_feature_ray_query                   = (1u << 4),
    pelagia_feature_mesh_shader                 = (1u << 5),
    pelagia_feature_decode_av1                  = (1u << 6),
    pelagia_feature_decode_h264                 = (1u << 7),
};

#ifdef PELAGIA_D3D12
/** The entry point to a Windows Direct 3D 12 Ultimate rendering backend. */
AMWAPI RIVEN_ENCORE(pelagia, d3d12);
#endif
#ifdef PELAGIA_METAL
/** The entry point to an Apple Metal3 rendering backend. */
AMWAPI RIVEN_ENCORE(pelagia, metal);
#endif
#ifdef PELAGIA_WEBGPU
/** The entry point to a Vulkan rendering backend. */
AMWAPI RIVEN_ENCORE(pelagia, webgpu);
#endif
#ifdef PELAGIA_VULKAN
/** The entry point to a WebGPU rendering backend. */
AMWAPI RIVEN_ENCORE(pelagia, vulkan);
#endif

/** The entry point to a stub rendering backend, available on all platforms.
 *  This is a valid API that provides a zeroed-out configuration. */
AMWAPI RIVEN_ENCORE(pelagia, stub);

/** Executes native encores until one writes to the interface:
 *  - d3d12 - Windows platforms with DX12 drivers.
 *  - metal - Apple platforms with Metal 3 drivers (Apple Silicon).
 *  - vulkan - All platforms (except Emscripten) with Vulkan 1.2+ drivers.
 *  - webgpu - WebAssembly builds, or expose layers from native implementations I guess.
 *
 *  Available encores are tried in order:
 *      native option (d3d12 -> metal -> webgpu) -> vulkan
 *
 *  If all options fail, the stub interface may be written as a fallback.
 *  This may be disabled by defining PELAGIA_DISABLE_STUB_FALLBACK.
 *
 *  Other possible rendering backends that could be created:
 *      GNM (PS5), NVN, AGC, serialized proxy, older APIs(OpenGL, OpenGL ES, DX11, etc.)? */
AMWAPI RIVEN_ENCORE(pelagia, native);

/** Quered information about the physical device, used either for logging or for decisions
 *  related to creating rendering devices. This information must be passed to the device 
 *  create info, when creating a device. */
struct pelagia_physical_device_info {
    struct str              name;           /**< The name of the physical device. */
    u32                     index;          /**< Internal index of the physical device structure. */
    u32                     device_type;    /**< Device type, e.g. discrete GPU, integrated GPU, CPU, virtual. */
    u32                     vendor_id;      /**< The vendor ID, e.g. AMD, NVIDIA, Intel. */
    u32                     api_version;    /**< The version of the supported API, depends on the backend. */
    u32                     features;       /**< Bits from enum pelagia_features. */
    usize                   total_vram;     /**< Total video memory available, in bytes. */
    /** An internally resolved score for comparing devices with each other. By itself a score 
     *  means nothing, as backend can write arbitrary scores as they do the device query. */
    u32                     score;
    /** This device has surface capabilities and can be used to create and control swapchains. */
    b32                     presentation;
};

/** Information needed to create a rendering device. */
struct pelagia_device_create_info {
    struct work_header                          work_header;            /**< Holds the error code and work index. */
    struct pelagia                             *pelagia;                /**< The context for the device. */
    struct pelagia_device                     **write_device;           /**< The device handle will be written here. */
    const struct pelagia_physical_device_info  *physical_device;        /**< This physical device will be used. */
    struct riven_allocation                     allocation;             /**< The host allocation that will hold all device state. The caller may pass a riven_tag, or do the deed after a query. */
    u32                                         frames_in_flight;       /**< GPU resources buffering, must be between 1 and PELAGIA_MAX_FRAMES_IN_FLIGHT. */
};

/** Types of GPU resources. */
enum pelagia_resource_type {
    pelagia_resource_type_invalid = 0u,
    pelagia_resource_type_device_memory,
    pelagia_resource_type_buffer,
    pelagia_resource_type_texture,
    pelagia_resource_type_sampler,
    pelagia_resource_type_shader,
    pelagia_resource_type_pipeline_layout,
    pelagia_resource_type_graphics_pipeline,
    pelagia_resource_type_compute_pipeline,
    pelagia_resource_type_raytracing_pipeline,
    pelagia_resource_type_shader_binding_table,
    pelagia_resource_type_descriptor_set,
    pelagia_resource_type_query_pool,
    pelagia_resource_type_swapchain,
    pelagia_resource_type_bottom_level,
    pelagia_resource_type_top_level,
    pelagia_resource_type_count,
};
#define pelagia_resource_type_index(__type) \
    (pelagia_resource_type_##__type - 1u)

/** All resource types implement this header as the first struct member,
 *  so that we can safely cast any resource type into this header. */
struct pelagia_resource_header {
    /** The device used to create this GPU resource. */
    struct pelagia_device                  *device; 
    /** Indicates the type of the internal resource structure. */
    enum pelagia_resource_type              type;
    /** A render handle it references. */
    render_resource_t                       handle;
};

/** Abstracts the handle to a GPU resource. */
union pelagia_resource {
    struct pelagia_resource_header         *header;
    struct pelagia_device_memory           *device_memory;
    struct pelagia_buffer                  *buffer;
    struct pelagia_texture                 *texture;
    struct pelagia_sampler                 *sampler;
    struct pelagia_shader                  *shader;
    struct pelagia_pipeline_layout         *pipeline_layout;
    struct pelagia_graphics_pipeline       *graphics_pipeline;
    struct pelagia_compute_pipeline        *compute_pipeline;
    struct pelagia_raytracing_pipeline     *raytracing_pipeline;
    struct pelagia_shader_binding_table    *shader_binding_table;
    struct pelagia_descriptor_set          *descriptor_set;
    struct pelagia_query_pool              *query_pool;
    struct pelagia_swapchain               *swapchain;
    struct pelagia_bottom_level            *bottom_level;
    struct pelagia_top_level               *top_level;
    void                                   *write;
};

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
enum pelagia_texture_format {
    pelagia_texture_format_unknown = 0,
    pelagia_texture_format_r1_unorm,        /**< 1-bit no format */
    pelagia_texture_format_a8_unorm,        /**< 8-bit alpha */
    /* depth formats */
    pelagia_texture_format_d16_unorm,       /**< 16-bit unorm depth */
    pelagia_texture_format_d16s8_unorm,     /**< 24-bit unorm depth/stencil */
    pelagia_texture_format_d24s8_unorm,     /**< 32-bit unorm depth/stencil */
    pelagia_texture_format_d32_float,       /**< 32-bit float depth */
    pelagia_texture_format_d0s8_unorm,      /**< 8-bit unorm stencil */
    /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
    pelagia_texture_format_r8_snorm,
    pelagia_texture_format_r8_unorm,
    pelagia_texture_format_r8_sint,
    pelagia_texture_format_r8_uint,
    /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
    pelagia_texture_format_r16_float,
    pelagia_texture_format_r16_snorm,
    pelagia_texture_format_r16_unorm,
    pelagia_texture_format_r16_sint,
    pelagia_texture_format_r16_uint,
    pelagia_texture_format_r8g8_snorm,
    pelagia_texture_format_r8g8_unorm,
    pelagia_texture_format_r8g8_sint,
    pelagia_texture_format_r8g8_uint,
    /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
    pelagia_texture_format_r8g8b8_snorm,
    pelagia_texture_format_r8g8b8_unorm,
    pelagia_texture_format_r8g8b8_sint,
    pelagia_texture_format_r8g8b8_uint,
    /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
    pelagia_texture_format_r32_float,
    pelagia_texture_format_r32_sint,
    pelagia_texture_format_r32_uint,
    pelagia_texture_format_r16g16_float,
    pelagia_texture_format_r16g16_snorm,
    pelagia_texture_format_r16g16_unorm,
    pelagia_texture_format_r16g16_sint,
    pelagia_texture_format_r16g16_uint,
    pelagia_texture_format_r8g8b8a8_snorm,
    pelagia_texture_format_r8g8b8a8_unorm,
    pelagia_texture_format_r8g8b8a8_sint,
    pelagia_texture_format_r8g8b8a8_uint,
    pelagia_texture_format_b8g8r8a8_snorm,
    pelagia_texture_format_b8g8r8a8_unorm,
    pelagia_texture_format_b8g8r8a8_sint,
    pelagia_texture_format_b8g8r8a8_uint,
    pelagia_texture_format_r10g10b10a2,
    pelagia_texture_format_b10g10r10a2,
    /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r16g16b16_float,
    pelagia_texture_format_r16g16b16_snorm,
    pelagia_texture_format_r16g16b16_unorm,
    pelagia_texture_format_r16g16b16_sint,
    pelagia_texture_format_r16g16b16_uint,
    /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r64_float,
    pelagia_texture_format_r64_sint,
    pelagia_texture_format_r64_uint,
    pelagia_texture_format_r32g32_float,
    pelagia_texture_format_r32g32_sint,
    pelagia_texture_format_r32g32_uint,
    pelagia_texture_format_r16g16b16a16_float,
    pelagia_texture_format_r16g16b16a16_snorm,
    pelagia_texture_format_r16g16b16a16_unorm,
    pelagia_texture_format_r16g16b16a16_sint,
    pelagia_texture_format_r16g16b16a16_uint,
    /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r32g32b32_float,
    pelagia_texture_format_r32g32b32_sint,
    pelagia_texture_format_r32g32b32_uint,
    /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r32g32b32a32_float,
    pelagia_texture_format_r32g32b32a32_sint,
    pelagia_texture_format_r32g32b32a32_uint,
    pelagia_texture_format_r64g64_float,
    pelagia_texture_format_r64g64_sint,
    pelagia_texture_format_r64g64_uint,
    /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r64g64b64_float,
    pelagia_texture_format_r64g64b64_sint,
    pelagia_texture_format_r64g64b64_uint,
    /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
    pelagia_texture_format_r64g64b64a64_float,
    pelagia_texture_format_r64g64b64a64_sint,
    pelagia_texture_format_r64g64b64a64_uint,
    /* compressed texture formats */
    pelagia_texture_format_BC1,         /**< DXT1 r5g6b5a1 */
    pelagia_texture_format_BC2,         /**< DXT3 r5g6b6a4 */
    pelagia_texture_format_BC3,         /**< DXT5 r5g6b5a8 */
    pelagia_texture_format_BC4,         /**< LATC1/ATI1 r8 */
    pelagia_texture_format_BC5,         /**< LATC2/ATI2 r8g8 */
    pelagia_texture_format_BC6H,        /**< BC6H r16g16b16_float */
    pelagia_texture_format_BC7,         /**< BC7 rgb 4-7 bits per color channel */
    pelagia_texture_format_ETC1,        /**< ETC1 r8g8b8 */
    pelagia_texture_format_ETC2,        /**< ETC2 r8g8b8 */
    pelagia_texture_format_ETC2A,       /**< ETC2 r8g8b8a8 */
    pelagia_texture_format_ETC2A1,      /**< ETC2 r8g8b8a1 */
    pelagia_texture_format_PTC12,       /**< PVRTC1 rgb 2BPP */
    pelagia_texture_format_PTC14,       /**< PVRTC1 rgb 4BPP */
    pelagia_texture_format_PTC22,       /**< PVRTC2 rgba 2BPP */
    pelagia_texture_format_PTC24,       /**< PVRTC2 rgba 4BPP */
    pelagia_texture_format_ASTC4x4,     /**< ASTC 4x4 8.0 BPP */
    pelagia_texture_format_ASTC5x4,     /**< ASTC 5x4 6.40 BPP */
    pelagia_texture_format_ASTC5x5,     /**< ASTC 5x5 5.12 BPP */
    pelagia_texture_format_ASTC6x5,     /**< ASTC 6x5 4.27 BPP */
    pelagia_texture_format_ASTC6x6,     /**< ASTC 6x6 3.56 BPP */
    pelagia_texture_format_ASTC8x5,     /**< ASTC 8x5 3.20 BPP */
    pelagia_texture_format_ASTC8x6,     /**< ASTC 8x6 2.67 BPP */
    pelagia_texture_format_ASTC8x8,     /**< ASTC 8x8 2.0 BPP */
    pelagia_texture_format_ASTC10x5,    /**< ASTC 10x5 2.56 BPP */
    pelagia_texture_format_ASTC10x6,    /**< ASTC 10x6 2.13 BPP */
    pelagia_texture_format_ASTC10x8,    /**< ASTC 10x8 1.60 BPP */
    pelagia_texture_format_ASTC10x10,   /**< ASTC 10x10 1.28 BPP */
    pelagia_texture_format_ASTC12x10,   /**< ASTC 12x10 1.07 BPP */
    pelagia_texture_format_ASTC12x12,   /**< ASTC 12x12 0.89 BPP */
    /* format count */
    pelagia_texture_format_count,
};

/** Macro to define a pelagia 'create_info' structure's header. */
#define PELAGIA_RESOURCES_CREATE_INFO_HEADER(__type) \
    struct work_header                      work_header;    /**< Holds the error code and work index. */ \
    struct pelagia_device                  *device;         /**< Creates resources on the GPU, will write itself to the resources' headers. */ \
    enum pelagia_resource_type              resource_type;  /**< The type of this create info, used for abstracted access. */ \
    u32                                     write_count;    /**< The length of the write array, equal to amount of resources to be created. */ \
    struct pelagia_##__type               **write;          /**< An array of handles where the resources will be written. They will share the same lifetime. */ \
    struct riven_allocation                 allocation;     /**< Allocation query of memory requirements. */

/** Abstracts common fields in a create_info structure of any resource type. */
struct pelagia_resources_create_info { PELAGIA_RESOURCES_CREATE_INFO_HEADER(resource_header); };

/** Information needed to create a GPU allocation. */
struct pelagia_device_memory_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(device_memory);
    /* TODO */
};

/** Information needed to create an array of buffers. */
struct pelagia_buffers_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(buffer);
    /* TODO */
};

/** Information needed to create an array of textures. */
struct pelagia_textures_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(texture);
    /* TODO */
};

/** Information needed to create an array of samplers. */
struct pelagia_samplers_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(sampler);
    /* TODO */
};

/** Information needed to create an array of shader modules. */
struct pelagia_shaders_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(sampler);
    /* TODO */
};

/** Information needed to create a pipeline layout. */
struct pelagia_pipeline_layouts_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(pipeline_layout);
    /* TODO */
};

/** Information needed to create a graphics pipeline. */
struct pelagia_graphics_pipelines_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(graphics_pipeline);
    /* TODO */
};

/** Information needed to create a compute pipeline. */
struct pelagia_compute_pipelines_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(compute_pipeline);
    /* TODO */
};

/** Information needed to create a raytracing pipeline. */
struct pelagia_raytracing_pipelines_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(raytracing_pipeline);
    /* TODO */
};

/** Information needed to create a shader binding table for specialized raytracing pipelines shader groups. */
struct pelagia_shader_binding_tables_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(shader_binding_table);
    /* TODO */
};

/** Information needed to create an array of descriptor sets. */
struct pelagia_descriptor_sets_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(descriptor_set);
    /* TODO */
};

/** Information needed to create an array of query pools. */
struct pelagia_query_pools_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(query_pool);
    /* TODO */
};

/** Information needed to create an array of swapchains. */
struct pelagia_swapchain_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(swapchain);
    /** We need to interface with the display backend to create a surface we can draw to. */
    const struct hadal         *hadal;
    /** If a given format is supported by the surface and the physical device, it is preferred. */
    enum pelagia_texture_format preferred_format;
    /** If true, V-SYNC will be enabled (FIFO present mode is used). */
    b32                         enable_vsync;
};

/** Information needed to create an array of bottom-level acceleration structures. */
struct pelagia_bottom_levels_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(bottom_level);
    /* TODO */
};

/** Information needed to create an array of top-level acceleration structures. */
struct pelagia_top_levels_create_info {
    PELAGIA_RESOURCES_CREATE_INFO_HEADER(top_level);
    /* TODO */
};

/** Writes information about the physical devices available. This info can be used for both logging GPU information 
 *  and for choosing physical devices whom rendering devices will be created from. The information about physical 
 *  devices has been already obtained during the encore as part of the initialization process - in this procedure 
 *  we only write high-level info. The caller must allocate host memory for the physical device infos. Hadal is 
 *  passed to create a temporary surface and assert surface capabilities. It can be NULL but may prevent you from 
 *  creating a swapchain later, as it is necessary to interface the display backend to check for presentation.
 *
 *  The first call performs a query to retrieve all information relevant to the physical devices 
 *  present in the rendering backend. After this data is available to us, any subsequent calls 
 *  will write public high-level information needed to make decisions about creating a pelagia_device. 
 *
 *  If out_physical_devices is not NULL, it must point into an array of physical_device_info, 
 *  that will store information for every physical device. Their count may be retrieved from 
 *  out_physical_device_count, and the caller may allocate memory for the physical device infos.
 *
 *  Non-zero value is returned on errors that invalidate the query, or the rendering backend. */
typedef s32 (AMWCALL *PFN_pelagia_query_physical_devices)(
    struct pelagia                      *pelagia, 
    const struct hadal                  *hadal,
    u32                                 *out_device_count, 
    struct pelagia_physical_device_info *out_devices);

/** Creates a rendering device. The given physical device configuration must be valid, it can be obtained from the initial query. */
typedef void (AMWCALL *PFN_pelagia_create_device)(struct pelagia_device_create_info *create_info);

/** All existing resources with an affinity of this device must be destroyed before the device may be destroyed too. */
typedef void (AMWCALL *PFN_pelagia_destroy_device)(struct pelagia_device *device);

/** Macro to define a procedure pointer that creates a type of GPU resource. */
#define PFN_PELAGIA_CREATE_RESOURCE(type) \
    typedef void (AMWCALL *PFN_pelagia_create_##type)(struct pelagia_##type##_create_info *create_info)

/** Allocated GPU memory. */
PFN_PELAGIA_CREATE_RESOURCE(device_memory);

/** Creates a buffer. */
PFN_PELAGIA_CREATE_RESOURCE(buffers);

/** Creates a texture. */
PFN_PELAGIA_CREATE_RESOURCE(textures);

/** Creates a sampler. */
PFN_PELAGIA_CREATE_RESOURCE(samplers);

/** Creates a shader module. */
PFN_PELAGIA_CREATE_RESOURCE(shaders);

/** Creates a pipeline layout. */
PFN_PELAGIA_CREATE_RESOURCE(pipeline_layouts);

/** Creates graphics pipelines from an array of requests in the pipeline create info. */
PFN_PELAGIA_CREATE_RESOURCE(graphics_pipelines);

/** Creates compute pipelines from an array of requests in the pipeline create info. */
PFN_PELAGIA_CREATE_RESOURCE(compute_pipelines);

/** Creates raytracing pipelines from an array of requests in the pipeline create info. */
PFN_PELAGIA_CREATE_RESOURCE(raytracing_pipelines);

/** Creates a command buffer. */
PFN_PELAGIA_CREATE_RESOURCE(shader_binding_tables);

/** Creates a descriptor set. */
PFN_PELAGIA_CREATE_RESOURCE(descriptor_sets);

/** Creates a query pool. */
PFN_PELAGIA_CREATE_RESOURCE(query_pools);

/** Creates a swapchain. The given device must support presentation to a window surface. */
PFN_PELAGIA_CREATE_RESOURCE(swapchain);

/** Creates a bottom-level acceleration structure. */
PFN_PELAGIA_CREATE_RESOURCE(bottom_levels);

/** Creates a top-level acceleration structure. */
PFN_PELAGIA_CREATE_RESOURCE(top_levels);

/** Bundles together resources of a shared lifetime for bulk cleanup. */
struct pelagia_destroy_resources_work {
    struct work_header          header;
    union pelagia_resource     *resources;
    u32                         resource_count;
};

/** All GPU resources can be destroyed using this procedure. An implementation may use a dispatch table. */
typedef void (AMWCALL *PFN_pelagia_destroy_resources)(struct pelagia_destroy_resources_work *work);

/** Procedures to be provided by an implementation. The backend must implement the 'struct pelagia' and 
 *  put the pelagia_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the rendering backend and the public interface defined below. */
struct pelagia_interface {
    struct riven_interface_header               header;

    PFN_pelagia_query_physical_devices          query_physical_devices;
    PFN_pelagia_create_device                   create_device;
    PFN_pelagia_destroy_device                  destroy_device;

    PFN_pelagia_create_device_memory            create_device_memory;
    PFN_pelagia_create_buffers                  create_buffers;
    PFN_pelagia_create_textures                 create_textures;
    PFN_pelagia_create_samplers                 create_samplers;
    PFN_pelagia_create_shaders                  create_shaders;
    PFN_pelagia_create_pipeline_layouts         create_pipeline_layouts;
    PFN_pelagia_create_graphics_pipelines       create_graphics_pipelines;
    PFN_pelagia_create_compute_pipelines        create_compute_pipelines;
    PFN_pelagia_create_raytracing_pipelines     create_raytracing_pipelines;
    PFN_pelagia_create_shader_binding_tables    create_shader_binding_tables;
    PFN_pelagia_create_descriptor_sets          create_descriptor_sets;
    PFN_pelagia_create_query_pools              create_query_pools;
    PFN_pelagia_create_swapchain                create_swapchain;
    PFN_pelagia_create_bottom_levels            create_bottom_levels;
    PFN_pelagia_create_top_levels               create_top_levels;
    PFN_pelagia_destroy_resources               destroy_resources;
};

attr_inline attr_nonnull(1,3)
s32 pelagia_query_physical_devices(
    struct pelagia                      *pelagia,
    const struct hadal                  *hadal,
    u32                                 *out_device_count,
    struct pelagia_physical_device_info *out_devices)
{
    struct pelagia_interface *interface = (struct pelagia_interface *)pelagia;
    assert_debug(interface->query_physical_devices);
    return interface->query_physical_devices(pelagia, hadal, out_device_count, out_devices);
}

#ifdef __cplusplus
}
#endif
