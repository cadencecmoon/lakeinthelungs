#pragma once

#if !defined(MOON_GPU_TABLE_SET_BINDING)
#define MOON_GPU_TABLE_SET_BINDING 0
#define MOON_STORAGE_BUFFER_BINDING 0
#define MOON_STORAGE_TEXTURE_BINDING 1
#define MOON_SAMPLED_TEXTURE_BINDING 2
#define MOON_SAMPLER_BINDING 3
#define MOON_DEVICE_ADDRESS_BUFFER_BINDING 4
#define MOON_ACCELERATION_STRUCTURE_BINDING 5
#endif

/* TODO d3d12, metal bindings ? */
#define MOON_DECL_STORAGE_BUFFERS           [[vk::binding(MOON_STORAGE_BUFFER_BINDING, 0)]]
#define MOON_DECL_STORAGE_TEXTURES          [[vk::binding(MOON_STORAGE_TEXTURE_BINDING, 0)]]
#define MOON_DECL_SAMPLED_TEXTURES          [[vk::binding(MOON_SAMPLED_TEXTURE_BINDING, 0)]]
#define MOON_DECL_SAMPLERS                  [[vk::binding(MOON_SAMPLER_BINDING, 0)]]
#define MOON_DECL_ACCELERATION_STRUCTURES   [[vk::binding(MOON_ACCELERATION_STRUCTURE_BINDING, 0)]]
#define MOON_DECL_BUFFER_ADDRESSES          [[vk::binding(MOON_DEVICE_ADDRESS_BUFFER_BINDING, 0)]]

#define MOON_ID_INDEX_BITS 20
#define MOON_ID_INDEX_MASK ((uint64_t(1) << MOON_ID_INDEX_BITS) - uint64_t(1))
#define MOON_ID_INDEX_OFFSET 0
#define MOON_ID_VERSION_BITS 44
#define MOON_ID_VERSION_MASK ((uint64_t(1) << MOON_ID_VERSION_BITS) - uint64_t(1))
#define MOON_ID_VERSION_OFFSET MOON_ID_INDEX_BITS

#define _MOON_DECL_VEC_TYPES(SLANG_TYPE, MOON_TYPE) \
    typedef SLANG_TYPE MOON_TYPE;                   \
    typedef SLANG_TYPE ## 1 MOON_TYPE ## vec1;      \
    typedef SLANG_TYPE ## 2 MOON_TYPE ## vec2;      \
    typedef SLANG_TYPE ## 3 MOON_TYPE ## vec3;      \
    typedef SLANG_TYPE ## 4 MOON_TYPE ## vec4;

#define _MOON_DECL_MAT_TYPES(SLANG_TYPE, MOON_TYPE)      \
    typedef matrix<SLANG_TYPE, 1, 1> MOON_TYPE ## mat1x1 \
    typedef matrix<SLANG_TYPE, 2, 1> MOON_TYPE ## mat1x2 \
    typedef matrix<SLANG_TYPE, 3, 1> MOON_TYPE ## mat1x3 \
    typedef matrix<SLANG_TYPE, 4, 1> MOON_TYPE ## mat1x4 \
    typedef matrix<SLANG_TYPE, 1, 2> MOON_TYPE ## mat2x1 \
    typedef matrix<SLANG_TYPE, 2, 2> MOON_TYPE ## mat2x2 \
    typedef matrix<SLANG_TYPE, 3, 2> MOON_TYPE ## mat2x3 \
    typedef matrix<SLANG_TYPE, 4, 2> MOON_TYPE ## mat2x4 \
    typedef matrix<SLANG_TYPE, 1, 3> MOON_TYPE ## mat3x1 \
    typedef matrix<SLANG_TYPE, 2, 3> MOON_TYPE ## mat3x2 \
    typedef matrix<SLANG_TYPE, 3, 3> MOON_TYPE ## mat3x3 \
    typedef matrix<SLANG_TYPE, 4, 3> MOON_TYPE ## mat3x4 \
    typedef matrix<SLANG_TYPE, 1, 4> MOON_TYPE ## mat4x1 \
    typedef matrix<SLANG_TYPE, 2, 4> MOON_TYPE ## mat4x2 \
    typedef matrix<SLANG_TYPE, 3, 4> MOON_TYPE ## mat4x3 \
    typedef matrix<SLANG_TYPE, 4, 4> MOON_TYPE ## mat4x4

#define _MOON_DECL_VEC_MAT_8_16_32_64_TYPES(SLANG_TYPE, MOON_TYPE) \
    _MOON_DECL_VEC_TYPES(SLANG_TYPE ## 8_t,  MOON_TYPE ## 8)       \
    _MOON_DECL_VEC_TYPES(SLANG_TYPE ## 16_t, MOON_TYPE ## 16)      \
    _MOON_DECL_VEC_TYPES(SLANG_TYPE ## 32_t, MOON_TYPE ## 32)      \
    _MOON_DECL_VEC_TYPES(SLANG_TYPE ## 64_t, MOON_TYPE ## 64)      \
    _MOON_DECL_MAT_TYPES(SLANG_TYPE ## 8_t,  MOON_TYPE ## 8)       \
    _MOON_DECL_MAT_TYPES(SLANG_TYPE ## 16_t, MOON_TYPE ## 16)      \
    _MOON_DECL_MAT_TYPES(SLANG_TYPE ## 32_t, MOON_TYPE ## 32)      \
    _MOON_DECL_MAT_TYPES(SLANG_TYPE ## 64_t, MOON_TYPE ## 64)

namespace moon {
    _MOON_DECL_VEC_MAT_8_16_32_64_TYPES(uint, u)
    _MOON_DECL_VEC_MAT_8_16_32_64_TYPES(int, s)
    _MOON_DECL_VEC_TYPES(half, f16)
    _MOON_DECL_VEC_TYPES(float, f32)
    _MOON_DECL_VEC_TYPES(double, f64)
    _MOON_DECL_MAT_TYPES(half, f16)
    _MOON_DECL_MAT_TYPES(float, f32)
    _MOON_DECL_MAT_TYPES(double, f64)
    _MOON_DECL_VEC_TYPES(bool, b32)
    _MOON_DECL_MAT_TYPES(bool, b32)

    MOON_DECL_STORAGE_BUFFERS           __DynamicResource               buffers[];
    MOON_DECL_STORAGE_BUFFERS           coherent __DynamicResource      buffers_coherent[];
    MOON_DECL_STORAGE_TEXTURES          __DynamicResource               storage_textures[];
    MOON_DECL_STORAGE_TEXTURES          coherent __DynamicResource      storage_textures_coherent[];
    MOON_DECL_SAMPLED_TEXTURES          __DynamicResource               sampled_textures[];
    MOON_DECL_SAMPLED_TEXTURES          coherent __DynamicResource      sampled_textures_coherent[];
    MOON_DECL_SAMPLERS                  SamplerState                    samplers[];
    MOON_DECL_ACCELERATION_STRUCTURES   RaytracingAccelerationStructure acceleration_structures[];
    MOON_DECL_BUFFER_ADDRESSES          StructuredBuffer<moon::u64>     buffer_addresses;

    struct buffer_id {
        moon::u64 value;

        moon::u32 index() {
            return moon::u32(MOON_ID_INDEX_MASK & value);
        }
        moon::u64 version() {
            return (value >> MOON_ID_VERSION_OFFSET);
        }
        moon::b32 is_empty() {
            return value == 0;
        }
        moon::u64 device_address() {
            return buffer_addresses[index()];
        }
    };

    struct texture_view_id {
        moon::u64 value;

        moon::u32 index() {
            return moon::u32(MOON_ID_INDEX_MASK & value);
        }
        moon::u64 version() {
            return (value >> MOON_ID_VERSION_OFFSET);
        }
        moon::b32 is_empty() {
            return value == 0;
        }
    };

    struct texture_view_idx {
        moon::u32 value;

        moon::u32 index() {
            return moon::u32(MOON_ID_INDEX_MASK & value);
        }
        moon::b32 is_empty() {
            return value == 0;
        }
    };

    struct sampler_id {
        moon::u64 value;

        moon::u32 index() {
            return moon::u32(MOON_ID_INDEX_MASK & value);
        }
        moon::u64 version() {
            return (value >> MOON_ID_VERSION_OFFSET);
        }
        moon::b32 is_empty() {
            return value == 0;
        }
        SamplerState get() {
            return samplers[index()];
        }
    };

#if defined(MOON_RAY_TRACING)
    MOON_DECL_ACCELERATION_STRUCTURES RaytracingAccelerationStructure ray_tracing_acceleration_structure_table[];
    extension RaytracingAccelerationStructure {
        static RaytracingAccelerationStructure get(moon::tlas i) {
            return ray_tracing_acceleration_structure_table[i.index()];
        }
    };

    struct tlas_id {
        moon::u64 value;
        moon::u32 index() {
            return xaku_u32(MOON_ID_INDEX_MASK & value);
        }
        moon::u64 version() {
            return (value >> MOON_ID_VERSION_OFFSET);
        }
        moon::b32 is_empty() {
            return value == 0;
        }
        RaytracingAccelerationStructure get() {
            return ray_tracing_acceleration_structure_table[index()];
        }
    };
#endif // LAKE_RAY_TRACING

    extension ByteAddressBuffer {
        static ByteAddressBuffer get(moon::buffer_id i) {
            return buffers[i.index()];
        }
        static ByteAddressBuffer get_coherent(moon::buffer_id i) {
            return buffers_coherent[i.index()];
        }
    };

    extension RWByteAddressBuffer {
        static RWByteAddressBuffer get(moon::buffer_id i) {
            return buffers[i.index()];
        }
        static RWByteAddressBuffer get_coherent(moon::buffer_id i) {
            return buffers_coherent[i.index()];
        }
    };

    extension SamplerState {
        static SamplerState get(moon::sampler_id i) {
            return samplers[i.index()];
        }
    };
} // namespace moon

#define _MOON_DECL_TEX_EXTENSION(TYPE, TABLE)                           \
__generic<T : ITexelElement> extension TYPE<T> {                        \
    static TYPE<T> get(moon::texture_view_id i) {                       \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(moon::texture_view_id i) {              \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
    static TYPE<T> get(moon::texture_view_idx i) {                      \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(moon::texture_view_idx i) {             \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
}                                                                       \
namespace moon {                                                        \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _id                                                  \
    {                                                                   \
        texture_view_id handle;                                         \
        TYPE<T> get() { return TYPE<T>::get(handle); }                  \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(handle); }\
    };                                                                  \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _index                                               \
    {                                                                   \
        texture_view_idx index;                                         \
        TYPE<T> get() { return TYPE<T>::get(index); }                   \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(index); } \
    };                                                                  \
} // namespace moon

_MOON_DECL_TEX_EXTENSION(RWTexture1D, moon::storage_textures)
_MOON_DECL_TEX_EXTENSION(RWTexture2D, moon::storage_textures)
_MOON_DECL_TEX_EXTENSION(RWTexture3D, moon::storage_textures)
_MOON_DECL_TEX_EXTENSION(RWTexture1DArray, moon::storage_textures)
_MOON_DECL_TEX_EXTENSION(RWTexture2DArray, moon::storage_textures)

_MOON_DECL_TEX_EXTENSION(Texture1D, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(Texture2D, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(Texture3D, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(Texture1DArray, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(Texture2DArray, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(TextureCube, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(TextureCubeArray, moon::sampled_textures)
_MOON_DECL_TEX_EXTENSION(Texture2DMS, moon::sampled_textures)
