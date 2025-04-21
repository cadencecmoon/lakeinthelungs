#pragma once

#if !defined(XAKU_GPU_TABLE_SET_BINDING)
#define XAKU_GPU_TABLE_SET_BINDING 0
#define XAKU_STORAGE_BUFFER_BINDING 0
#define XAKU_STORAGE_TEXTURE_BINDING 1
#define XAKU_SAMPLED_TEXTURE_BINDING 2
#define XAKU_SAMPLER_BINDING 3
#define XAKU_DEVICE_ADDRESS_BUFFER_BINDING 4
#define XAKU_ACCELERATION_STRUCTURE_BINDING 5
#endif

#define XAKU_DECL_STORAGE_BUFFERS           [[vk::binding(XAKU_STORAGE_BUFFER_BINDING, 0)]]
#define XAKU_DECL_STORAGE_TEXTURES          [[vk::binding(XAKU_STORAGE_TEXTURE_BINDING, 0)]]
#define XAKU_DECL_SAMPLED_TEXTURES          [[vk::binding(XAKU_SAMPLED_TEXTURE_BINDING, 0)]]
#define XAKU_DECL_SAMPLERS                  [[vk::binding(XAKU_SAMPLER_BINDING, 0)]]
#define XAKU_DECL_ACCELERATION_STRUCTURES   [[vk::binding(XAKU_ACCELERATION_STRUCTURE_BINDING, 0)]]

#define XAKU_ID_INDEX_BITS 20
#define XAKU_ID_INDEX_MASK ((uint64_t(1) << XAKU_ID_INDEX_BITS) - uint64_t(1))
#define XAKU_ID_INDEX_OFFSET 0
#define XAKU_ID_VERSION_BITS 44
#define XAKU_ID_VERSION_MASK ((uint64_t(1) << XAKU_ID_VERSION_BITS) - uint64_t(1))
#define XAKU_ID_VERSION_OFFSET XAKU_ID_INDEX_BITS

#define _XAKU_DECL_VEC_TYPES(SLANG_TYPE, XAKU_TYPE) \
    typedef SLANG_TYPE XAKU_TYPE;                   \
    typedef SLANG_TYPE ## 1 XAKU_TYPE ## vec1;      \
    typedef SLANG_TYPE ## 2 XAKU_TYPE ## vec2;      \
    typedef SLANG_TYPE ## 3 XAKU_TYPE ## vec3;      \
    typedef SLANG_TYPE ## 4 XAKU_TYPE ## vec4;

#define _XAKU_DECL_MAT_TYPES(SLANG_TYPE, XAKU_TYPE)      \
    typedef matrix<SLANG_TYPE, 1, 1> XAKU_TYPE ## mat1x1 \
    typedef matrix<SLANG_TYPE, 2, 1> XAKU_TYPE ## mat1x2 \
    typedef matrix<SLANG_TYPE, 3, 1> XAKU_TYPE ## mat1x3 \
    typedef matrix<SLANG_TYPE, 4, 1> XAKU_TYPE ## mat1x4 \
    typedef matrix<SLANG_TYPE, 1, 2> XAKU_TYPE ## mat2x1 \
    typedef matrix<SLANG_TYPE, 2, 2> XAKU_TYPE ## mat2x2 \
    typedef matrix<SLANG_TYPE, 3, 2> XAKU_TYPE ## mat2x3 \
    typedef matrix<SLANG_TYPE, 4, 2> XAKU_TYPE ## mat2x4 \
    typedef matrix<SLANG_TYPE, 1, 3> XAKU_TYPE ## mat3x1 \
    typedef matrix<SLANG_TYPE, 2, 3> XAKU_TYPE ## mat3x2 \
    typedef matrix<SLANG_TYPE, 3, 3> XAKU_TYPE ## mat3x3 \
    typedef matrix<SLANG_TYPE, 4, 3> XAKU_TYPE ## mat3x4 \
    typedef matrix<SLANG_TYPE, 1, 4> XAKU_TYPE ## mat4x1 \
    typedef matrix<SLANG_TYPE, 2, 4> XAKU_TYPE ## mat4x2 \
    typedef matrix<SLANG_TYPE, 3, 4> XAKU_TYPE ## mat4x3 \
    typedef matrix<SLANG_TYPE, 4, 4> XAKU_TYPE ## mat4x4

#define _XAKU_DECL_VEC_MAT_8_16_32_64_TYPES(SLANG_TYPE, XAKU_TYPE) \
    _XAKU_DECL_VEC_TYPES(SLANG_TYPE ## 8_t,  XAKU_TYPE ## 8)       \
    _XAKU_DECL_VEC_TYPES(SLANG_TYPE ## 16_t, XAKU_TYPE ## 16)      \
    _XAKU_DECL_VEC_TYPES(SLANG_TYPE ## 32_t, XAKU_TYPE ## 32)      \
    _XAKU_DECL_VEC_TYPES(SLANG_TYPE ## 64_t, XAKU_TYPE ## 64)      \
    _XAKU_DECL_MAT_TYPES(SLANG_TYPE ## 8_t,  XAKU_TYPE ## 8)       \
    _XAKU_DECL_MAT_TYPES(SLANG_TYPE ## 16_t, XAKU_TYPE ## 16)      \
    _XAKU_DECL_MAT_TYPES(SLANG_TYPE ## 32_t, XAKU_TYPE ## 32)      \
    _XAKU_DECL_MAT_TYPES(SLANG_TYPE ## 64_t, XAKU_TYPE ## 64)

namespace xaku {
    _XAKU_DECL_VEC_MAT_8_16_32_64_TYPES(uint, u)
    _XAKU_DECL_VEC_MAT_8_16_32_64_TYPES(int, s)
    _XAKU_DECL_VEC_TYPES(half, f16)
    _XAKU_DECL_VEC_TYPES(float, f32)
    _XAKU_DECL_VEC_TYPES(double, f64)
    _XAKU_DECL_MAT_TYPES(half, f16)
    _XAKU_DECL_MAT_TYPES(float, f32)
    _XAKU_DECL_MAT_TYPES(double, f64)
    _XAKU_DECL_VEC_TYPES(bool, b32)
    _XAKU_DECL_MAT_TYPES(bool, b32)

    XAKU_DECL_STORAGE_BUFFERS           __DynamicResource               buffers[];
    XAKU_DECL_STORAGE_BUFFERS           coherent __DynamicResource      buffers_coherent[];
    XAKU_DECL_STORAGE_TEXTURES          __DynamicResource               storage_textures[];
    XAKU_DECL_STORAGE_TEXTURES          coherent __DynamicResource      storage_textures_coherent[];
    XAKU_DECL_SAMPLED_TEXTURES          __DynamicResource               sampled_textures[];
    XAKU_DECL_SAMPLED_TEXTURES          coherent __DynamicResource      sampled_textures_coherent[];
    XAKU_DECL_SAMPLERS                  SamplerState                    samplers[];
    XAKU_DECL_ACCELERATION_STRUCTURES   RaytracingAccelerationStructure acceleration_structures[];

    [[vk::binding(XAKU_DEVICE_ADDRESS_BUFFER_BINDING, 0)]] StructuredBuffer<xaku_u64> buffer_addresses;
    struct buffer_id {
        xaku_u64 value;

        xaku_u32 index() {
            return xaku_u32(XAKU_ID_INDEX_MASK & value);
        }
        xaku_u64 version() {
            return (value >> XAKU_ID_VERSION_OFFSET);
        }
        xaku_b32 is_empty() {
            return value == 0;
        }
        xaku_u64 device_address() {
            return buffer_addresses[index()];
        }
    };

    struct texture_view_id {
        xaku_u64 value;

        xaku_u32 index() {
            return xaku_u32(XAKU_ID_INDEX_MASK & value);
        }
        xaku_u64 version() {
            return (value >> XAKU_ID_VERSION_OFFSET);
        }
        xaku_b32 is_empty() {
            return value == 0;
        }
    };

    struct texture_view_index {
        xaku_u32 value;

        xaku_u32 index() {
            return xaku_u32(XAKU_ID_INDEX_MASK & value);
        }
        xaku_b32 is_empty() {
            return value == 0;
        }
    };

    struct sampler_id {
        xaku_u64 value;

        xaku_u32 index() {
            return xaku_u32(XAKU_ID_INDEX_MASK & value);
        }
        xaku_u64 version() {
            return (value >> XAKU_ID_VERSION_OFFSET);
        }
        xaku_b32 is_empty() {
            return value == 0;
        }
        SamplerState get() {
            return samplers[index()];
        }
    };

#if defined(XAKU_RAY_TRACING)
    [[vk::binding(XAKU_ACCELERATION_STRUCTURE_BINDING, 0)]] RaytracingAccelerationStructure ray_tracing_acceleration_structure_table[];
    extension RaytracingAccelerationStructure {
        static RaytracingAccelerationStructure get(xaku::tlas_id i) {
            return ray_tracing_acceleration_structure_table[i.index()];
        }
    };

    struct tlas_id {
        xaku_u64 value;
        xaku_u32 index() {
            return xaku_u32(XAKU_ID_INDEX_MASK & value);
        }
        xaku_u64 version() {
            return (value >> XAKU_ID_VERSION_OFFSET);
        }
        xaku_b32 is_empty() {
            return value == 0;
        }
        RaytracingAccelerationStructure get() {
            return ray_tracing_acceleration_structure_table[index()];
        }
    };
#endif // XAKU_RAY_TRACING

    extension ByteAddressBuffer {
        static ByteAddressBuffer get(xaku::buffer_id i) {
            return buffers[i.index()];
        }
        static ByteAddressBuffer get_coherent(xaku::buffer_id i) {
            return buffers_coherent[i.index()];
        }
    };

    extension RWByteAddressBuffer {
        static RWByteAddressBuffer get(xaku::buffer_id i) {
            return buffers[i.index()];
        }
        static RWByteAddressBuffer get_coherent(xaku::buffer_id i) {
            return buffers_coherent[i.index()];
        }
    };

    extension SamplerState {
        static SamplerState get(xaku::sampler_id i) {
            return samplers[i.index()];
        }
    };
} // namespace xaku

#define _XAKU_DECL_TEX_EXTENSION(TYPE, TABLE)                           \
__generic<T : ITexelElement> extension TYPE<T> {                        \
    static TYPE<T> get(xaku::texture_view_id i) {                       \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(xaku::texture_view_id i) {              \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
    static TYPE<T> get(xaku::texture_view_index i) {                    \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(xaku::texture_view_index i) {           \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
}                                                                       \
namespace xaku {                                                        \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _id                                                  \
    {                                                                   \
        texture_view_id id;                                             \
        TYPE<T> get() { return TYPE<T>::get(id); }                      \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(id); }    \
    };                                                                  \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _index                                               \
    {                                                                   \
        texture_view_index index;                                       \
        TYPE<T> get() { return TYPE<T>::get(index); }                   \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(index); } \
    };                                                                  \
}

_XAKU_DECL_TEX_EXTENSION(RWTexture1D, xaku::storage_textures)
_XAKU_DECL_TEX_EXTENSION(RWTexture2D, xaku::storage_textures)
_XAKU_DECL_TEX_EXTENSION(RWTexture3D, xaku::storage_textures)
_XAKU_DECL_TEX_EXTENSION(RWTexture1DArray, xaku::storage_textures)
_XAKU_DECL_TEX_EXTENSION(RWTexture2DArray, xaku::storage_textures)

_XAKU_DECL_TEX_EXTENSION(Texture1D, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(Texture2D, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(Texture3D, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(Texture1DArray, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(Texture2DArray, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(TextureCube, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(TextureCubeArray, xaku::sampled_textures)
_XAKU_DECL_TEX_EXTENSION(Texture2DMS, xaku::sampled_textures)

// Remove these later?
_XAKU_DECL_VEC_MAT_8_16_32_64_TYPES(uint, xaku_u)
_XAKU_DECL_VEC_MAT_8_16_32_64_TYPES(int, xaku_s)
_XAKU_DECL_VEC_TYPES(half, xaku_f16)
_XAKU_DECL_VEC_TYPES(float, xaku_f32)
_XAKU_DECL_VEC_TYPES(double, xaku_f64)
_XAKU_DECL_MAT_TYPES(half, xaku_f16)
_XAKU_DECL_MAT_TYPES(float, xaku_f32)
_XAKU_DECL_MAT_TYPES(double, xaku_f64)
_XAKU_DECL_VEC_TYPES(bool, xaku_b32)
_XAKU_DECL_MAT_TYPES(bool, xaku_b32)
typedef xaku::buffer_id         xaku_buffer_id;
typedef xaku::texture_view_id   xaku_texture_view_id;
typedef xaku::sampler_id        xaku_sampler_id;
#if defined(XAKU_RAY_TRACING)
typedef xaku::tlas_id           xaku_tlas_id;
#endif // XAKU_RAY_TRACING
#define xaku_RWBufferPtr(STRUCT_TYPE) Ptr<STRUCT_TYPE>
#define xaku_BufferPtr(STRUCT_TYPE) Ptr<STRUCT_TYPE>
#define XAKU_DECL_BUFFER_PTR_ALIGN(STRUCT_TYPE, ALIGN)
#define XAKU_DECL_BUFFER_PTR(STRUCT_TYPE)
#define deref(PTR) (*PTR)
#define deref_idx(PTR, INDEX) PTR[INDEX]
#define as_address(PTR) uint64_t(PTR)
