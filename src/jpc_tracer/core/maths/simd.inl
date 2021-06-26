#include "simd.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <emmintrin.h>
#include <xmmintrin.h>
namespace jpctracer
{
namespace simd
{
//******************************************************************************
//*                        Loads                                               *
//******************************************************************************

// Floats
inline VECTOR SIMD_CALLCONV LoadFloat(const float* source) noexcept
{
    assert(source);
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source[0], 0, 0, 0};

#elif defined(_SIMD_SSE_AVAILABLE)
    return _mm_load_ss(source);
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat2(const float* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source[0], source[1], 0, 0};
#elif defined(_SIMD_SSE_AVAILABLE)
    return _mm_castpd_ps(_mm_load_sd((const double*)source));
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat2Aligend(const Float2A* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source->x, source->y, 0, 0};
#elif defined(_SIMD_SSE_AVAILABLE)
    return _mm_castpd_ps(_mm_load_sd((const double*)source));
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat3(const float* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source[0], source[1], source[2], 0};
#elif defined(_SIMD_SSE_AVAILABLE)
    __m128 xy = LoadFloat2(source);
    __m128 z = LoadFloat(source + 2);
    return _mm_movelh_ps(xy, z);
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat3Aligned(const Float3A* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source->x, source->y, source->z, 0};
#elif defined(_SIMD_SSE_AVAILABLE)
    __m128 float4 = _mm_load_ps(&source->x);
    return _mm_and_ps(float4, g_mask1110);
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat4(const float* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source[0], source[1], source[2], source[3]};
#elif defined(_SIMD_SSE_AVAILABLE)
    return _mm_loadu_ps(source);
#endif
}
inline VECTOR SIMD_CALLCONV LoadFloat4Aligned(const Float4A* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source->x, source->y, source->z, source->w};
#elif defined(_SIMD_SSE_AVAILABLE)
    return _mm_load_ps(&source->x);

#endif
}

// Ints
inline VECTOR SIMD_CALLCONV LoadUInt4Aligend(const UInt4A* source) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {source->x, source->y, source->z, source->w};
#elif defined(_SIMD_SSE_AVAILABLE)
    __m128i v_uint4 = _mm_load_si128((const __m128i*)&source->x);
    return _mm_castsi128_ps(v_uint4);
#endif
}

//******************************************************************************
//*                        Store                                               *
//******************************************************************************

// Floats
inline void SIMD_CALLCONV StoreFloat(FCVECTOR v_float, float* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination[0] = v_float.f[0];
#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_store_ss(destination, v_float);
#endif
}
inline void SIMD_CALLCONV StoreFloat2(FCVECTOR v_float, float* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination[0] = v_float.f[0];
    destination[1] = v_float.f[1];
#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_store_sd((double*)destination, _mm_castps_pd(v_float));
#endif
}
inline void SIMD_CALLCONV StoreFloat2Aligend(FCVECTOR v_float, Float2A* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination->x = v_float.f[0];
    destination->y = v_float.f[1];
#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_store_sd((double*)destination, _mm_castps_pd(v_float));
#endif
}
inline void SIMD_CALLCONV StoreFloat3(FCVECTOR v_float, float* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination[0] = v_float.f[0];
    destination[1] = v_float.f[1];
    destination[2] = v_float.f[2];
#elif defined(_SIMD_SSE4_AVAILABLE)
    *(int*)(destination) = _mm_extract_ps(v_float, 0);
    *(int*)(destination + 1) = _mm_extract_ps(v_float, 1);
    *(int*)(destination + 2) = _mm_extract_ps(v_float, 2);
#elif defined(_SIMD_SSE_AVAILABLE)
    StoreFloat2(v_float, destination);
    _mm_shuffle_ps(v_float, v_float, _MM_SHUFFLE(2, 2, 2, 2));
#endif
}
inline void SIMD_CALLCONV StoreFloat3Aligned(FCVECTOR v_float, Float3A* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination->x = v_float.f[0];
    destination->y = v_float.f[1];
    destination->z = v_float.f[2];
#elif defined(_SIMD_SSE4_AVAILABLE)
    _mm_store_sd((double*)(&destination->x), _mm_castps_pd(v_float));
    *(int*)(&destination->z) = _mm_extract_ps(v_float, 2);
#elif defined(_SIMD_SSE_AVAILABLE)
    StoreFloat2(v_float, destination);
    _mm_shuffle_ps(v_float, v_float, _MM_SHUFFLE(2, 2, 2, 2));

#endif
}
inline void SIMD_CALLCONV StoreFloat4(FCVECTOR v_float, float* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination[0] = v_float.f[0];
    destination[1] = v_float.f[1];
    destination[2] = v_float.f[2];
    destination[3] = v_float.f[3];
#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_storeu_ps(destination, v_float);
#endif
}
inline void SIMD_CALLCONV StoreFloat4Aligned(FCVECTOR v_float, Float4A* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination->x = v_float.f[0];
    destination->y = v_float.f[1];
    destination->z = v_float.f[2];
    destination->w = v_float.f[3];
#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_store_ps(&destination->x, v_float);
#endif
}

// Ints
inline void SIMD_CALLCONV StoreUInt4Aligend(FCVECTOR v_int, UInt4A* destination) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    destination->x = v_int.i[0];
    destination->y = v_int.i[1];
    destination->z = v_int.i[2];
    destination->w = v_int.i[3];

#elif defined(_SIMD_SSE_AVAILABLE)
    _mm_store_si128((__m128i*)destination, _mm_castps_si128(v_int));
#endif
}

//******************************************************************************
//*                     Gerneral operations                                    *
//******************************************************************************

inline VECTOR SIMD_CALLCONV VectorZeros() noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {0.0f, 0.0f, 0.0f, 0.0f};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorFull(float fill_value) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {fill_value, fill_value, fill_value, fill_value};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSet(float x, float y, float z, float w) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {x, y, z, w};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorTrueInt() noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::uint32_t(1), std::uint32_t(1), std::uint32_t(1), std::uint32_t(1)};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorFalseInt() noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::uint32_t(0), std::uint32_t(0), std::uint32_t(0), std::uint32_t(0)};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetInt(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t w) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {x, y, z, w};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline float SIMD_CALLCONV VectorGetByIndex(FCVECTOR v, std::size_t i) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.f[i];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline float SIMD_CALLCONV VectorGetX(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.f[0];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline float SIMD_CALLCONV VectorGetY(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.f[1];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline float SIMD_CALLCONV VectorGetZ(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.f[2];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline float SIMD_CALLCONV VectorGetW(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.f[3];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline std::uint32_t SIMD_CALLCONV VectorGetIntByIndex(FCVECTOR v, std::size_t i) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.i[i];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline std::uint32_t SIMD_CALLCONV VectorGetIntX(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.i[0];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline std::uint32_t SIMD_CALLCONV VectorGetIntY(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.i[1];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline std::uint32_t SIMD_CALLCONV VectorGetIntZ(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.i[2];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline std::uint32_t SIMD_CALLCONV VectorGetIntW(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return v.i[3];
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorSetByIndex(FCVECTOR v, float f, std::size_t i) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.f[i] = f;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetX(FCVECTOR v, float x) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.f[0] = x;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetY(FCVECTOR v, float y) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.f[1] = y;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetZ(FCVECTOR v, float z) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.f[2] = z;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetW(FCVECTOR v, float w) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.f[3] = w;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorSetIntByIndex(FCVECTOR v, uint32_t x, size_t i) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.i[i] = x;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetIntX(FCVECTOR v, uint32_t x) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.i[0] = x;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetIntY(FCVECTOR v, uint32_t y) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.i[1] = y;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetIntZ(FCVECTOR v, uint32_t z) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.i[2] = z;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSetIntW(FCVECTOR v, uint32_t w) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = v;
    result.i[3] = w;
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorReplicateX(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[0], v.f[0], v.f[0], v.f[0]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorReplicateY(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[1], v.f[1], v.f[1], v.f[1]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorReplicateZ(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[2], v.f[2], v.f[2], v.f[2]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorReplicateW(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[3], v.f[3], v.f[3], v.f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorCompose(FCVECTOR v1, FCVECTOR v2, std::uint32_t i0, std::uint32_t i1,
                                          std::uint32_t i2, std::uint32_t i3) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {
        i0 < 4 ? v1.f[i0] : v2.f[i0-4],
        i1 < 4 ? v1.f[i1] : v2.f[i1-4],
        i2 < 4 ? v1.f[i2] : v2.f[i2-4],
        i3 < 4 ? v1.f[i3] : v2.f[i3-4]
    };
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
template <std::uint32_t i0, std::uint32_t i1, std::uint32_t i2, std::uint32_t i3>
inline VECTOR SIMD_CALLCONV VectorCompose(FCVECTOR v1, FCVECTOR v2) noexcept
{

#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {
        i0 < 4 ? v1.f[i0] : v2.f[i0-4],
        i1 < 4 ? v1.f[i1] : v2.f[i1-4],
        i2 < 4 ? v1.f[i2] : v2.f[i2-4],
        i3 < 4 ? v1.f[i3] : v2.f[i3-4]
    };
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// selection either 0 or 1
inline VECTOR SIMD_CALLCONV VectorSetMask(uint32_t select_1, uint32_t select_2, uint32_t select_3,
                                          uint32_t select_4) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    std::uint32_t elements[2] = {g_unselect, G_SELECT};
    VECTOR result;
    result.i[0] = elements[select_1];
    result.i[1] = elements[select_2];
    result.i[2] = elements[select_3];
    result.i[3] = elements[select_4];
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

// returns (v[i0],v[i1],v[i2],v[i3])
inline VECTOR SIMD_CALLCONV VectorReorder(FCVECTOR v, uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[i0], v.f[i1], v.f[i2], v.f[i3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
template <std::uint32_t i0, std::uint32_t i1, std::uint32_t i2, std::uint32_t i3>
inline VECTOR SIMD_CALLCONV VectorReorder(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[i0], v.f[i1], v.f[i2], v.f[i3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSelect(FCVECTOR v1, FCVECTOR v2, FCVECTOR mask) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return
    {
        mask.i[0] == 0 ? v1.f[0] : v2.f[0],
        mask.i[1] == 0 ? v1.f[1] : v2.f[1],
        mask.i[2] == 0 ? v1.f[2] : v2.f[2],
        mask.i[3] == 0 ? v1.f[3] : v2.f[3]
    };
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// returns (v1[0],v1[1],v2[0],v2[1])
inline VECTOR SIMD_CALLCONV VectorMergeXY(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.f[0], v1.f[1], v2.f[0], v2.f[1]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// returns (v1[2],v1[3],v2[2],v2[3])
inline VECTOR SIMD_CALLCONV VectorMergeZW(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.f[2], v1.f[3], v2.f[2], v2.f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorRotateLeft(FCVECTOR v, uint32_t elements) noexcept
{
    // & 3 like % 4
    return VectorReorder(v, elements & 3, (elements + 1) & 3, (elements + 2) & 3, (elements + 3) & 3);
}

inline VECTOR SIMD_CALLCONV VectorRotateRight(FCVECTOR v, uint32_t elements) noexcept
{
    return VectorReorder(v, (4 - elements) & 3, (5 - elements) & 3, (6 - elements) & 3, (7 - elements) & 3);
}

inline VECTOR SIMD_CALLCONV VectorEqual(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] == v2.f[0], v1.f[1] == v2.f[1], v1.f[2] == v2.f[2], v1.f[3] == v2.f[3]);

#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorEqualInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.i[0] == v2.i[0], v1.i[1] == v2.i[1], v1.i[2] == v2.i[2], v1.i[3] == v2.i[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorNearEqual(FCVECTOR v1, FCVECTOR v2, FCVECTOR epsilon) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    float diff_x = v1.f[0] - v2.f[0];
    float diff_y = v1.f[1] - v2.f[1];
    float diff_z = v1.f[2] - v2.f[2];
    float diff_w = v1.f[3] - v2.f[3];
    int is_eq_x = fabs(diff_x) < epsilon.f[0];
    int is_eq_y = fabs(diff_y) < epsilon.f[1];
    int is_eq_z = fabs(diff_z) < epsilon.f[2];
    int is_eq_w = fabs(diff_w) < epsilon.f[3];
    return VectorSetMask(is_eq_x, is_eq_y, is_eq_z, is_eq_w);

#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorNotEqual(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] != v2.f[0], v1.f[1] != v2.f[1], v1.f[2] != v2.f[2], v1.f[3] != v2.f[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorNotEqualInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.i[0] != v2.i[0], v1.i[1] != v2.i[1], v1.i[2] != v2.i[2], v1.i[3] != v2.i[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorGreater(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] > v2.f[0], v1.f[1] > v2.f[1], v1.f[2] > v2.f[2], v1.f[3] > v2.f[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorGreaterOrEqual(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] >= v2.f[0], v1.f[1] >= v2.f[1], v1.f[2] >= v2.f[2], v1.f[3] >= v2.f[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLess(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] < v2.f[0], v1.f[1] < v2.f[1], v1.f[2] < v2.f[2], v1.f[3] < v2.f[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLessOrEqual(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(v1.f[0] <= v2.f[0], v1.f[1] > v2.f[1], v1.f[2] > v2.f[2], v1.f[3] > v2.f[3]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// -bounds <= v <= bounds
inline VECTOR SIMD_CALLCONV VectorInBounds(FCVECTOR v, FCVECTOR bounds) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return VectorSetMask(-bounds.f[0] <= v.f[0] && v.f[0] <= bounds.f[0],
                         -bounds.f[1] <= v.f[1] && v.f[1] <= bounds.f[1],
                         -bounds.f[2] <= v.f[2] && v.f[2] <= bounds.f[2],
                         -bounds.f[3] <= v.f[3] && v.f[3] <= bounds.f[3]);
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorIsNaN(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(std::isnan(v.f[0]), std::isnan(v.f[1]), std::isnan(v.f[2]), std::isnan(v.f[3]));
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorIsInfinite(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return VectorSetMask(std::isinf(v.f[0]), std::isinf(v.f[1]), std::isinf(v.f[2]), std::isinf(v.f[3]));
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline VECTOR SIMD_CALLCONV VectorMin(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::min(v1.f[0], v2.f[0]), std::min(v1.f[1], v2.f[1]), std::min(v1.f[2], v2.f[2]),
            std::min(v1.f[3], v2.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorMax(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::max(v1.f[0], v2.f[0]), std::max(v1.f[1], v2.f[1]), std::max(v1.f[2], v2.f[2]),
            std::max(v1.f[3], v2.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorRound(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::round(v.f[0]), std::round(v.f[1]), std::round(v.f[2]), std::round(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// like floor for positive Numbers and like ceil for negative Numbers
inline VECTOR SIMD_CALLCONV VectorTruncate(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::trunc(v.f[0]), std::trunc(v.f[1]), std::trunc(v.f[2]), std::trunc(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorFloor(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::floor(v.f[0]), std::floor(v.f[1]), std::floor(v.f[2]), std::floor(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorCeil(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::ceil(v.f[0]), std::floor(v.f[1]), std::floor(v.f[2]), std::floor(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorClamp(FCVECTOR v, FCVECTOR min, FCVECTOR max) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::clamp(v.f[0], min.f[0], max.f[0]), std::clamp(v.f[1], min.f[1], max.f[1]),
            std::clamp(v.f[2], min.f[2], max.f[2]), std::clamp(v.f[3], min.f[3], max.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// Pseudocode
// VECTOR Result
//
// Result.x = min(max(V1.x, 0.0f), 1.0f)
// Result.y = min(max(V1.y, 0.0f), 1.0f)
// Result.z = min(max(V1.z, 0.0f), 1.0f)
// Result.w = min(max(V1.w, 0.0f), 1.0f)
//
// return Result
inline VECTOR SIMD_CALLCONV VectorSaturate(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result;

    result.f[0] = std::min(std::max(v.f[0], 0.0f), 1.0f);
    result.f[1] = std::min(std::max(v.f[1], 0.0f), 1.0f);
    result.f[2] = std::min(std::max(v.f[2], 0.0f), 1.0f);
    result.f[3] = std::min(std::max(v.f[3], 0.0f), 1.0f);

    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

// Logical
// ~v1
inline VECTOR SIMD_CALLCONV VectorNotInt(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {~v.i[0], ~v.i[1], ~v.i[2], ~v.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1 & v2
inline VECTOR SIMD_CALLCONV VectorAndInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] & v2.i[0], v1.i[1] & v2.i[1], v1.i[2] & v2.i[2], v1.i[3] & v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1 & ~ v2
inline VECTOR SIMD_CALLCONV VectorAndNotInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] & ~v2.i[0], v1.i[1] & ~v2.i[1], v1.i[2] & ~v2.i[2], v1.i[3] & ~v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1 | v2
inline VECTOR SIMD_CALLCONV VectorOrInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] | v2.i[0], v1.i[1] | v2.i[1], v1.i[2] | v2.i[2], v1.i[3] | v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// ~(v1 | v2)
inline VECTOR SIMD_CALLCONV VectorNorInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {~(v1.i[0] | v2.i[0]), ~(v1.i[1] | v2.i[1]), ~(v1.i[2] | v2.i[2]), ~(v1.i[3] | v2.i[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1 ^ v2
inline VECTOR SIMD_CALLCONV VectorXorInt(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] ^ v2.i[0], v1.i[1] ^ v2.i[1], v1.i[2] ^ v2.i[2], v1.i[3] ^ v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

// -v
inline VECTOR SIMD_CALLCONV VectorNegate(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {-v.i[0], -v.i[1], -v.i[2], -v.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorAdd(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] + v2.i[0], v1.i[1] + v2.i[1], v1.i[2] + v2.i[2], v1.i[3] + v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// VECTOR Result
// Result[0] = Result[1] = Result[2] = Result[3] = v[0]+v[1]+v[2]+v[3]
inline VECTOR SIMD_CALLCONV VectorSum(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    float sum = v.f[0] + v.f[1] + v.f[2] + v.f[3];
    return {sum, sum, sum, sum};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSubtract(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] - v2.i[0], v1.i[1] - v2.i[1], v1.i[2] - v2.i[2], v1.i[3] - v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorMultiply(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] * v2.i[0], v1.i[1] * v2.i[1], v1.i[2] * v2.i[2], v1.i[3] * v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1 * v2 + v3
inline VECTOR SIMD_CALLCONV VectorMultiplyAdd(FCVECTOR v1, FCVECTOR v2, FCVECTOR v3) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] * v2.i[0] + v3.i[0], v1.i[1] * v2.i[1] + v3.i[1], v1.i[2] * v2.i[2] + v2.i[2],
            v1.i[3] * v2.i[3] + v3.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorDivide(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] / v2.i[0], v1.i[1] / v2.i[1], v1.i[2] / v2.i[2], v1.i[3] / v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v3 - v1 * v2
inline VECTOR SIMD_CALLCONV VectorNegativeMultiplySubtract(FCVECTOR v1, FCVECTOR v2, FCVECTOR v3) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v3.i[0] - v1.i[0] * v2.i[0], v3.i[1] - v1.i[1] * v2.i[1], v3.i[2] - v1.i[2] * v2.i[2],
            v3.i[3] - v1.i[3] * v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorScale(FCVECTOR v, float factor) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v.f[0] * factor, v.f[1] * factor, v.f[2] * factor, v.f[3] * factor};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// 1/v
inline VECTOR SIMD_CALLCONV VectorInverseEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {1.f / v.f[0], 1.f / v.f[1], 1.f / v.f[2], 1.f / v.f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorInverse(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {1.f / v.f[0], 1.f / v.f[1], 1.f / v.f[2], 1.f / v.f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSqrtEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::sqrt(v.f[0]), std::sqrt(v.f[1]), std::sqrt(v.f[2]), std::sqrt(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSqrt(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::sqrt(v.f[0]), std::sqrt(v.f[1]), std::sqrt(v.f[2]), std::sqrt(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorInverseSqrtEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {1.f / std::sqrt(v.f[0]), 1.f / std::sqrt(v.f[1]), 1.f / std::sqrt(v.f[2]), 1.f / std::sqrt(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorInverseSqrt(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {1.f / std::sqrt(v.f[0]), 1.f / std::sqrt(v.f[1]), 1.f / std::sqrt(v.f[2]), 1.f / std::sqrt(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorExp2(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::exp2(v.f[0]), std::exp2(v.f[1]), std::exp2(v.f[2]), std::exp2(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorExpE(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::exp(v.f[0]), std::exp(v.f[1]), std::exp(v.f[2]), std::exp(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLog2(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::log2(v.f[0]), std::log2(v.f[1]), std::log2(v.f[2]), std::log2(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLogE(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::log(v.f[0]), std::log(v.f[1]), std::log(v.f[2]), std::log(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorPow(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::pow(v1.f[0], v2.f[0]), std::pow(v1.f[1], v2.f[1]), std::pow(v1.f[2], v2.f[2]),
            std::pow(v1.f[3], v2.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorAbs(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::abs(v.f[0]), std::abs(v.f[1]), std::abs(v.f[2]), std::abs(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorMod(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {v1.i[0] % v2.i[0], v1.i[1] % v2.i[1], v1.i[2] % v2.i[2], v1.i[3] % v2.i[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSin(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::sin(v.f[0]), std::sin(v.f[1]), std::sin(v.f[2]), std::sin(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorSinEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::sin(v.f[0]), std::sin(v.f[1]), std::sin(v.f[2]), std::sin(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorCos(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::cos(v.f[0]), std::cos(v.f[1]), std::cos(v.f[2]), std::cos(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorCosEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::cos(v.f[0]), std::cos(v.f[1]), std::cos(v.f[2]), std::cos(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorTan(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::tan(v.f[0]), std::tan(v.f[1]), std::tan(v.f[2]), std::tan(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorTanEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::tan(v.f[0]), std::tan(v.f[1]), std::tan(v.f[2]), std::tan(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorASin(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::asin(v.f[0]), std::asin(v.f[1]), std::asin(v.f[2]), std::asin(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorASinEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::asin(v.f[0]), std::asin(v.f[1]), std::asin(v.f[2]), std::asin(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorACos(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::acos(v.f[0]), std::acos(v.f[1]), std::acos(v.f[2]), std::acos(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorACosEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::acos(v.f[0]), std::acos(v.f[1]), std::acos(v.f[2]), std::acos(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorATan(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::atan(v.f[0]), std::atan(v.f[1]), std::atan(v.f[2]), std::atan(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorATanEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::atan(v.f[0]), std::atan(v.f[1]), std::atan(v.f[2]), std::atan(v.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorATan2(FCVECTOR y, FCVECTOR x) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::atan2(x.f[0], y.f[0]), std::atan2(x.f[1], y.f[1]), std::atan2(x.f[2], y.f[2]),
            std::atan2(x.f[3], y.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorATan2Est(FCVECTOR y, FCVECTOR x) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::atan2(x.f[0], y.f[0]), std::atan2(x.f[1], y.f[1]), std::atan2(x.f[2], y.f[2]),
            std::atan2(x.f[3], y.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// Interpolation
// Linearinterpolate
// v0 + t * (v1 - v0)
inline VECTOR SIMD_CALLCONV VectorLerp(FCVECTOR v0, FCVECTOR v1, float t) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::lerp(v0.f[0], v1.f[0], t), std::lerp(v0.f[1], v1.f[1], t), std::lerp(v0.f[2], v1.f[2], t),
            std::lerp(v0.f[3], v1.f[3], t)};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLerpV(FCVECTOR v0, FCVECTOR v1, FCVECTOR v_t) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {std::lerp(v0.f[0], v1.f[0], v_t.f[0]), std::lerp(v0.f[1], v1.f[1], v_t.f[1]),
            std::lerp(v0.f[2], v1.f[2], v_t.f[2]), std::lerp(v0.f[3], v1.f[3], v_t.f[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

//******************************************************************************
//*                             Lineare algebra                                *
//******************************************************************************
// Vector3
inline VECTOR SIMD_CALLCONV Vector3Full(float fill_value) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {fill_value, fill_value, fill_value, 0};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV Vector3Zeros() noexcept
{
    return VectorZeros();
}
inline VECTOR SIMD_CALLCONV Vector3Orthogonal(FCVECTOR v) noexcept
{
    // Same as directxmath Vector3Orthogonal
    VECTOR zero = VectorZeros();
    VECTOR z = VectorReplicateZ(v);
    VECTOR yzyy = VectorReorder<G_Y_IDX, G_Z_IDX, G_Y_IDX, G_Y_IDX>(v);

    VECTOR negative_v = VectorNegate(v);
    VECTOR z_is_negative = VectorLess(z, zero);
    VECTOR xzyy_is_negative = VectorLess(yzyy, zero);

    VECTOR mask = VectorEqualInt(z_is_negative, xzyy_is_negative);

    VECTOR s = VectorAdd(yzyy, z);
    VECTOR d = VectorSubtract(yzyy, z);

    VECTOR r_0 = VectorCompose<G_COMPOSE_1X, G_COMPOSE_0X, G_COMPOSE_0X, G_COMPOSE_0X>(negative_v, s);
    VECTOR r_1 = VectorCompose<G_COMPOSE_1X, G_COMPOSE_0X, G_COMPOSE_0X, G_COMPOSE_0X>(v, d);

    return VectorSelect(r_1, r_0, mask);
}
inline VECTOR SIMD_CALLCONV Vector3Cross(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {
        v1.f[1] * v2.f[2] - v1.f[2] * v2.f[1],
        v1.f[2] * v2.f[0] - v1.f[0] * v2.f[2],
        v1.f[0] * v2.f[1] - v1.f[1] * v2.f[0],
        0
    };
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

// Vector
inline VECTOR SIMD_CALLCONV VectorDot(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    float sum = v1.f[0] * v2.f[0] + v1.f[1] * v2.f[1] + v1.f[2] * v2.f[2] + v1.f[3] * v2.f[3];
    return {sum, sum, sum, sum};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLengthSq(FCVECTOR v) noexcept
{
    return VectorDot(v, v);
}
inline VECTOR SIMD_CALLCONV VectorInverseLengthEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = VectorLengthEst(v);
    return VectorInverseEst(result);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorInverseLength(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = VectorLengthSq(v);
    return VectorInverse(result);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLengthEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = VectorLengthSq(v);
    return VectorSqrtEst(result);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorLength(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR result = VectorLengthSq(v);
    return VectorSqrt(result);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorNormalizeEst(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR inv_length = VectorInverseLengthEst(v);
    return VectorMultiply(v, inv_length);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline VECTOR SIMD_CALLCONV VectorNormalize(FCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    VECTOR inv_length = VectorInverseLength(v);
    return VectorMultiply(v, inv_length);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// result = incident - 2*dot(incident,normal)*normal
inline VECTOR SIMD_CALLCONV VectorReflect(FCVECTOR incident, FCVECTOR normal) noexcept
{
    VECTOR result = VectorDot(incident, normal);
    result = VectorAdd(result, result);
    result = VectorNegativeMultiplySubtract(result, normal, incident);
}
inline VECTOR SIMD_CALLCONV VectorAngleBetweenNormalsEst(FCVECTOR n1, FCVECTOR n2) noexcept
{
    VECTOR result = VectorDot(n1, n2);
    result = VectorClamp(result, G_NEGATIVE_ONE, G_ONE);
    result = VectorACosEst(result);
    return result;
}
inline VECTOR SIMD_CALLCONV VectorAngleBetweenNormals(FCVECTOR n1, FCVECTOR n2) noexcept
{
    VECTOR result = VectorDot(n1, n2);
    result = VectorClamp(result, G_NEGATIVE_ONE, G_ONE);
    result = VectorACos(result);
    return result;
}
inline VECTOR SIMD_CALLCONV VectorAngleBetweenVectors(FCVECTOR v1, FCVECTOR v2) noexcept
{
    VECTOR inv_length_1 = VectorInverseLength(v1);
    VECTOR inv_legnth_2 = VectorInverseLength(v2);
    VECTOR dot = VectorDot(v1, v2);
    VECTOR factor = VectorMultiply(inv_length_1, inv_legnth_2);
    VECTOR cos_angle = VectorMultiply(dot, factor);
    cos_angle = VectorClamp(cos_angle, G_NEGATIVE_ONE, G_ONE);
    return VectorACos(cos_angle);
}
inline VECTOR SIMD_CALLCONV Vector3Transform(FCMATRIX m, HCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {m.data[0].f[0] * v.f[0] + m.data[0].f[1] * v.f[1] + m.data[0].f[2] * v.f[2] + m.data[0].f[3],
            m.data[1].f[0] * v.f[0] + m.data[1].f[1] * v.f[1] + m.data[1].f[2] * v.f[2] + m.data[1].f[3],
            m.data[2].f[0] * v.f[0] + m.data[2].f[1] * v.f[1] + m.data[2].f[2] * v.f[2] + m.data[2].f[3],
            m.data[3].f[0] * v.f[0] + m.data[3].f[1] * v.f[1] + m.data[3].f[2] * v.f[2] + m.data[3].f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

#if (!defined(__clang__) && !defined(__GNUC__)) || defined(_SIMD_DISABLE_INTRINSICS)
VECTOR SIMD_CALLCONV operator+(FCVECTOR v) noexcept
{
    return v; //?
}
VECTOR SIMD_CALLCONV operator-(FCVECTOR v) noexcept
{
    return VectorNegate(v);
}

VECTOR& SIMD_CALLCONV operator+=(VECTOR& v1, FCVECTOR v2) noexcept
{
    v1 = VectorAdd(v1, v2);
    return v1;
}
VECTOR& SIMD_CALLCONV operator-=(VECTOR& v1, FCVECTOR v2) noexcept
{

    v1 = VectorSubtract(v1, v2);
    return v1;
}
VECTOR& SIMD_CALLCONV operator*=(VECTOR& v1, FCVECTOR v2) noexcept
{

    v1 = VectorMultiply(v1, v2);
    return v1;
}
VECTOR& SIMD_CALLCONV operator/=(VECTOR& v1, FCVECTOR v2) noexcept
{

    v1 = VectorDivide(v1, v2);
    return v1;
}

VECTOR& operator*=(VECTOR& v, float S) noexcept
{
    v = VectorMultiply(v, VectorFull(S));
    return v;
}
VECTOR& operator/=(VECTOR& v, float S) noexcept
{
    v = VectorDivide(v, VectorFull(S));
    return v;
}

VECTOR SIMD_CALLCONV operator+(FCVECTOR v1, FCVECTOR v2) noexcept
{
    return VectorAdd(v1, v2);
}
VECTOR SIMD_CALLCONV operator-(FCVECTOR v1, FCVECTOR v2) noexcept
{
    return VectorSubtract(v1, v2);
}
VECTOR SIMD_CALLCONV operator*(FCVECTOR v1, FCVECTOR v2) noexcept
{
    return VectorMultiply(v1, v2);
}
VECTOR SIMD_CALLCONV operator/(FCVECTOR v1, FCVECTOR v2) noexcept
{
    return VectorDivide(v1, v2);
}
VECTOR SIMD_CALLCONV operator*(FCVECTOR v, float S) noexcept
{
    return VectorMultiply(v, VectorFull(S));
}
VECTOR SIMD_CALLCONV operator*(float S, FCVECTOR v) noexcept
{
    return VectorMultiply(v, VectorFull(S));
}
VECTOR SIMD_CALLCONV operator/(FCVECTOR v, float S) noexcept
{
    return VectorDivide(v, VectorFull(S));
}
#endif

inline VECTOR SIMD_CALLCONV MatrixDotVector(FCMATRIX m, HCVECTOR v) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {m.data[0].f[0] * v.f[0] + m.data[0].f[1] * v.f[1] + m.data[0].f[2] * v.f[2] + m.data[0].f[3] * v.f[3],
            m.data[1].f[0] * v.f[0] + m.data[1].f[1] * v.f[1] + m.data[1].f[2] * v.f[2] + m.data[1].f[3] * v.f[3],
            m.data[2].f[0] * v.f[0] + m.data[2].f[1] * v.f[1] + m.data[2].f[2] * v.f[2] + m.data[2].f[3] * v.f[3],
            m.data[3].f[0] * v.f[0] + m.data[3].f[1] * v.f[1] + m.data[3].f[2] * v.f[2] + m.data[3].f[3] * v.f[3]};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

// Matrix
inline bool SIMD_CALLCONV MatrixIsNaN(FCMATRIX m) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    bool is_nan = false;
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            is_nan = is_nan && std::isnan(m.data[x].f[y]);
    return is_nan;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline bool SIMD_CALLCONV MatrixIsInfinite(FCMATRIX m) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    bool is_inf = false;
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            is_inf = is_inf && std::isinf(std::abs(m.data[x].f[y]));
    return is_inf;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixIdentity() noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
             {1.f, 0.f, 0.f, 0.f},
             {0.f, 1.f, 0.f, 0.f}, 
             {0.f, 0.f, 1.f, 0.f}, 
             {0.f, 0.f, 0.f, 1.f}
           }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixSet(float m00, float m01, float m02, float m03, float m10, float m11, float m12,
                                      float m13, float m20, float m21, float m22, float m23, float m30, float m31,
                                      float m32, float m33) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
             { m00, m01, m02, m03},
             { m10, m11, m12, m13}, 
             { m20, m21, m22, m23}, 
             { m30, m31, m32, m33}
           }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV Matrix3x3Set(float m00, float m01, float m02, float m10, float m11, float m12, float m20,
                                         float m21, float m22)
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
             { m00, m01, m02, 0.f},
             { m10, m11, m12, 0.f}, 
             { m20, m21, m22, 0.f}, 
             { 0.f, 0.f, 0.f, 1.f}
           }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline MATRIX SIMD_CALLCONV MatrixDotMatrix(FCMATRIX m1, GCMATRIX m2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return {MatrixDotVector(m1, m2.data[0]), MatrixDotVector(m1, m2.data[1]), MatrixDotVector(m1, m2.data[2]),
            MatrixDotVector(m1, m2.data[3])};
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixDotMatrixTranspose(FCMATRIX m1, GCMATRIX m2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    return MatrixDotMatrix(m1, MatrixTranspose(m2));
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixTranspose(FCMATRIX m) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {m.data[0].f[0], m.data[1].f[0], m.data[2].f[0], m.data[3].f[0]},
        {m.data[0].f[1], m.data[1].f[1], m.data[2].f[1], m.data[3].f[0]},
        {m.data[0].f[2], m.data[1].f[2], m.data[2].f[2], m.data[3].f[0]},
        {m.data[0].f[3], m.data[1].f[3], m.data[2].f[3], m.data[3].f[0]},
    }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV TransformationInverse(FCMATRIX m) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    /*

       1 0 0 x   m00 m01 m02 0   m00 m01 m02 x
    m =0 1 0 y * m10 m11 m12 0 = m10 m11 m12 y
       0 0 1 z   m20 m21 m22 0   m20 m21 m22 z
       0 0 0 1   0   0   0   1   0   0  0    1

        1 0 0 x   1 0 0 -x   1 0 0 0
        0 1 0 y * 0 1 0 -y = 0 1 0 0
        0 0 1 z   0 0 1 -z   0 0 1 0
        0 0 0 1   0 0 0  1   0 0 0 1


        inv00 inv01 inv02 0   1 0 0 -x   inv00 inv01 inv02 -x*inv00 - y*inv01 - z*inv02
        inv10 inv11 inv12 0 * 0 1 0 -y = inv10 inv11 inv12 -x*inv10 - y*inv11 - z*inv12
        inv20 inv21 inv22 0   0 0 1 -z   inv20 inv21 inv22 -z*inv20 - y*inv21 - z*inv22
        0     0     0     1   0 0 0  1   0     0     0      1
    */
    float m00 = m.data[0].f[0];
    float m01 = m.data[0].f[1];
    float m02 = m.data[0].f[2];
    float m10 = m.data[1].f[0];
    float m11 = m.data[1].f[1];
    float m12 = m.data[1].f[2];
    float m20 = m.data[2].f[0];
    float m21 = m.data[2].f[1];
    float m22 = m.data[2].f[2];

    float determinate = m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 -
                        m01 * m10 * m22 - m02 * m11 * m20;

    float inv00 = m11 * m22 - m12 * m21;
    float inv01 = m02 * m21 - m01 * m22;
    float inv02 = m01 * m12 - m02 * m11;
    float inv10 = m12 * m20 * m10 * m22;
    float inv11 = m00 * m22 - m02 * m20;
    float inv12 = m02 * m10 - m00 * m12;
    float inv20 = m10 * m21 - m11 * m20;
    float inv21 = m01 * m20 - m00 * m21;
    float inv22 = m00 * m11 - m01 * m10;

    float x = m.data[0].f[3];
    float y = m.data[1].f[3];
    float z = m.data[2].f[3];

    // clang-format off
    return{{
        {inv00, inv01, inv02, - x * inv00 - y * inv01 - z * inv02},
        {inv10, inv11, inv12, - x * inv10 - y * inv11 - z * inv12},
        {inv20, inv21, inv22, - x * inv20 - y * inv21 - z * inv22},
        {0.f  , 0.f  , 0.f  , 1.f}
            
    }};
    // clang-format on

#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
// v1*v2^T
inline MATRIX SIMD_CALLCONV MatrixVectorTensorProduct(FCVECTOR v1, FCVECTOR v2) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {v1.f[0]*v2.f[0],v1.f[0]*v2.f[1],v1.f[0]*v2.f[2],v1.f[0]*v2.f[3]},
        {v1.f[1]*v2.f[0],v1.f[1]*v2.f[1],v1.f[1]*v2.f[2],v1.f[1]*v2.f[3]},
        {v1.f[2]*v2.f[0],v1.f[2]*v2.f[1],v1.f[2]*v2.f[2],v1.f[2]*v2.f[3]},
        {v1.f[3]*v2.f[0],v1.f[3]*v2.f[1],v1.f[3]*v2.f[2],v1.f[3]*v2.f[3]}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

inline MATRIX SIMD_CALLCONV MatrixTranslation(float offset_x, float offset_y, float offset_z) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {1.f,0.f,0.f,offset_x},
        {0.f,1.f,0.f,offset_y},
        {0.f,0.f,1.f,offset_z},
        {0.f,0.f,0.f,1.f}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixTranslationFromVector(FCVECTOR offset) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {1.f,0.f,0.f,offset.f[0]},
        {0.f,1.f,0.f,offset.f[1]},
        {0.f,0.f,1.f,offset.f[2]},
        {0.f,0.f,0.f,1.f}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixScaling(float scale_x, float scale_y, float scale_z) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {scale_x, 0.f    , 0.f    , 0.f},
        {0.f    , scale_y, 0.f    , 0.f},
        {0.f    , 0.f    , scale_z, 0.f},
        {0.f    , 0.f    , 0.f    , 1.f}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixScalingFromVector(FCVECTOR scale) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {scale.f[0], 0.f       , 0.f       , 0.f},
        {0.f       , scale.f[1], 0.f       , 0.f},
        {0.f       , 0.f       , scale.f[2], 0.f},
        {0.f       , 0.f       , 0.f       , 1.f}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixRotationX(float angle) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {1.f, 0.f            ,   0.f            , 0.f},
        {0.f, std::cos(angle), - std::sin(angle), 0.f},
        {0.f, std::sin(angle),   std::cos(angle), 0.f},
        {0.f, 0.f            ,   0.f            , 1.f}
    }};
    // clang-foramt on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixRotationY(float angle) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
        {    std::cos(angle)  , 0.f, std::sin(angle), 0.f},
        {    0.f              , 1.f, 0.f            , 0.f},
        {    - std::sin(angle), 0.f, std::cos(angle), 0.f},
        {    0.f              , 0.f, 0.f            , 1.f}
    }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixRotationZ(float angle) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    // clang-format off
    return {{
            {std::cos(angle), -std::sin(angle), 0.f, 0.f},
            {std::sin(angle),  std::cos(angle), 0.f, 0.f},
            {0.f            ,  0.f            , 1.f, 0.f},
            {0.f            ,  0.f            , 0.f, 1.f}
    }};
    // clang-format on
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixRotationNormal(FCVECTOR normal) noexcept
{

    VECTOR temp_v = VectorSet(0, 0, -1, 0);
    VECTOR temp_v_2 = VectorSet(0, -1, 0, 0);
    VECTOR zero = VectorZeros();
    VECTOR v_z_is_zero = VectorNearEqual(temp_v, zero, VectorFull(0.0001));
    v_z_is_zero = VectorReplicateZ(v_z_is_zero);
    temp_v = VectorSelect(temp_v_2, temp_v, v_z_is_zero);

    VECTOR t1 = Vector3Cross(normal, temp_v);
    t1 = VectorNormalize(t1);
    VECTOR t2 = Vector3Cross(normal, t1);
    t2 = VectorNormalize(t2);

    MATRIX result;
    result.data[0] = t1;
    result.data[1] = t2;
    result.data[2] = normal;
    result.data[3] = VectorSet(0, 0, 0, 1);
    return result;
}
MATRIX SIMD_CALLCONV MatrixRotation(float angle_x, float angle_y, float angle_z) noexcept
{
#if defined(_SIMD_DISABLE_INTRINSICS)
    MATRIX result = MatrixRotationX(angle_x);
    MATRIX temp = MatrixRotationY(angle_y);
    result = MatrixDotMatrix(temp, result);
    temp = MatrixRotationZ(angle_z);
    result = MatrixDotMatrix(temp, result);
    return result;
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}

MATRIX SIMD_CALLCONV MatrixRotationFromVector(FCVECTOR angles) noexcept
{

#if defined(_SIMD_DISABLE_INTRINSICS)
    return MatrixRotation(angles.f[0], angles.f[1], angles.f[2]);
#elif defined(_SIMD_AVX_AVAILABLE)
#elif defined(_SIMD_SSE_AVAILABLE)
#endif
}
inline MATRIX SIMD_CALLCONV MatrixRotationScaleTranslation(FCVECTOR rotation, FCVECTOR scale, FCVECTOR offset)
{
    MATRIX result, temp;
    result = MatrixRotationFromVector(rotation);
    temp = MatrixScalingFromVector(scale);
    result = MatrixDotMatrix(temp, result);
    temp = MatrixTranslationFromVector(offset);
    result = MatrixDotMatrix(temp, result);
    return result;
}
} // namespace simd
} // namespace jpctracer
