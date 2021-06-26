#pragma once

//******************************************************************************
//*                      Check available intrinsics                            *
//******************************************************************************

#define _SIMD_DISABLE_INTRINSICS 1 // to disable all use of simd

#include <cstddef>
#include <cstdint>
#if defined(_MSC_VER)
#define SIMD_CALLCONV __vectorcall
#else
#define SIMD_CALLCONV
#endif

#if !defined(_SIMD_DISABLE_INTRINSICS)

//! defined(_SIMD_AVX2_AVAILABLE) so for testing it can be disabled
#if !defined(_SIMD_AVX2_AVAILABLE) && defined(__AVX2__)
#define _SIMD_AVX2_AVAILABLE
#endif

#if !defined(_SIMD_FMA3_AVAILABLE) && defined(__AVX2__)
#define _SIMD_FMA3_AVAILABLE
#endif

#if !defined(_SIMD_F16C_AVAILABLE) && (defined(__F16C__) || defined(__AVX__))
#define _SIMD_F16C_AVAILABLE
#endif

#if !defined(_SIMD_AVX_AVAILABLE) && (defined(__AVX2__) || defined(__AVX__) || defined(__F16C__))
#define _SIMD_AVX_AVAILABLE
#endif

#if !defined(_SIMD_SSE4_AVAILABLE) && defined(__AVX__)
#define _SIMD_SSE4_AVAILABLE
#endif

#if !defined(_SIMD_SSE3_AVAILABLE) && defined(__AVX__)
#define _SIMD_SSE3_AVAILABLE
#endif

#if !defined(_SIMD_SSE_AVAILABLE) && defined(__AVX__)
#define _SIMD_SSE_AVAILABLE
#endif

#endif

//******************************************************************************
//*                      Intrinsics includes                                   *
//******************************************************************************

#ifdef _SIMD_SSE_AVAILABLE
#include <emmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef _SIMD_SSE3_AVAILABLE
#include <pmmintrin.h>
#endif

#ifdef _SIMD_SSE4_AVAILABLE
#include <smmintrin.h>
#endif

#ifdef _SIMD_AVX_AVAILABLE
#include <immintrin.h>
#endif

//******************************************************************************
//*                      General includes                                      *
//******************************************************************************

#include <assert.h>
#include <cstdint>
#include <math.h>

#if defined(_SIMD_SSE_AVAILABLE) && !defined(_SIMD_DISABLE_INTRINSICS)
#define SIMD_STREAM_PS(p, a) _mm_stream_ps((p), (a))
#define SIMD256_STREAM_PS(p, a) _mm256_stream_ps((p), (a))
#define SIMD_SFENCE() _mm_sfence()

#if defined(_SIMD_FMA3_AVAILABLE)
#define SIMD_FMADD_PS(a, b, c) _mm_fmadd_ps((a), (b), (c))
#define SIMD_FNMADD_PS(a, b, c) _mm_fnmadd_ps((a), (b), (c))
#else
#define SIMD_FMADD_PS(a, b, c) _mm_add_ps(_mm_mul_ps((a), (b)), (c))
#define SIMD_FNMADD_PS(a, b, c) _mm_sub_ps((c), _mm_mul_ps((a), (b)))
#endif

#if defined(__clang__) || defined(__GNUC__)
#define SIMD_PREFETCH(a) __builtin_prefetch()
#else
#define SIMD_PREFETCH(a) __prefetch(a)
#endif

#endif

namespace jpctracer
{
namespace simd
{
#if defined(_SIMD_SSE_AVAILABLE) && !defined(_SIMD_DISABLE_INTRINSICS)
using VECTOR = __m128;
#else

struct __vector4
{
    union {
        float f[4];
        std::uint32_t i[4];
    };
    constexpr __vector4(float f1, float f2, float f3, float f4) noexcept : f{f1, f2, f3, f4}
    {
    }
    constexpr __vector4(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) : i{i1, i2, i3, i4}
    {
    }
    constexpr __vector4() = default;
};

using VECTOR = __vector4;
#endif
struct alignas(16) MATRIX
{
    VECTOR data[4];
};
// FCVECTOR is a const function Argument
#if (defined(__aarch64__) || defined(__x86_64__)) && !defined(_SIMD_DISABLE_INTRINSICS)
// 1-4 Args
using FCVECTOR = const VECTOR;
// 4 Arg
using GCVECTOR = const VECTOR;
// 5-6 Args
using HCVECTOR = const VECTOR;
// 7th+ Args
using ICVECTOR = const VECTOR&;

// 1 Arg
using FCMATRIX = const MATRIX;
// 2th+ Arg
using GCMATRIX = const MATRIX&;
#else
// 1-4 Args
using FCVECTOR = const VECTOR&;
// 4 Arg
using GCVECTOR = const VECTOR&;
// 5-6 Args
using HCVECTOR = const VECTOR&;
// 7th+ Args
using ICVECTOR = const VECTOR&;

// 1 Arg
using FCMATRIX = const MATRIX&;
// 2th+ Arg
using GCMATRIX = const MATRIX&;
#endif

// clang and gnu have overloads, msvc not
#if (!defined(__clang__) && !defined(__GNUC__)) || defined(_SIMD_DISABLE_INTRINSICS)
VECTOR SIMD_CALLCONV operator+(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV operator-(FCVECTOR v) noexcept;

VECTOR& SIMD_CALLCONV operator+=(VECTOR& v1, FCVECTOR v2) noexcept;
VECTOR& SIMD_CALLCONV operator-=(VECTOR& v1, FCVECTOR v2) noexcept;
VECTOR& SIMD_CALLCONV operator*=(VECTOR& v1, FCVECTOR v2) noexcept;
VECTOR& SIMD_CALLCONV operator/=(VECTOR& v1, FCVECTOR v2) noexcept;

VECTOR& operator*=(VECTOR& v, float S) noexcept;
VECTOR& operator/=(VECTOR& v, float S) noexcept;

VECTOR SIMD_CALLCONV operator+(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV operator-(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV operator*(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV operator/(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV operator*(FCVECTOR v, float S) noexcept;
VECTOR SIMD_CALLCONV operator*(float S, FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV operator/(FCVECTOR v, float S) noexcept;
#endif

//******************************************************************************
//*                   Efficent Datatypes for loading                           *
//******************************************************************************

constexpr std::size_t DataTypeAligment = 16;

struct alignas(DataTypeAligment) Float2A
{
    float x;
    float y;
};
struct alignas(DataTypeAligment) Float3A
{
    float x;
    float y;
    float z;
};
struct alignas(DataTypeAligment) Float4A
{
    float x;
    float y;
    float z;
    float w;
};
struct alignas(DataTypeAligment) UInt4A
{
    std::uint32_t x;
    std::uint32_t y;
    std::uint32_t z;
    std::uint32_t w;
};
struct alignas(DataTypeAligment) Matrix4x4
{
    union {
        struct
        {
            float _00, _01, _02, _03;
            float _10, _11, _12, _13;
            float _20, _21, _22, _23;
            float _30, _31, _32, _33;
        };
        float data[4][4];
    };
};
//******************************************************************************
//*                        Loads                                               *
//******************************************************************************

// Floats
VECTOR SIMD_CALLCONV LoadFloat(const float* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat2(const float* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat2Aligend(const Float2A* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat3(const float* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat3Aligned(const Float3A* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat4(const float* source) noexcept;
VECTOR SIMD_CALLCONV LoadFloat4Aligned(const Float4A* source) noexcept;

// Ints
VECTOR SIMD_CALLCONV LoadUInt4Aligend(const UInt4A* source) noexcept;

//******************************************************************************
//*                        Store                                               *
//******************************************************************************

// Floats
void SIMD_CALLCONV StoreFloat(FCVECTOR v_float, float* destination) noexcept;
void SIMD_CALLCONV StoreFloat2(FCVECTOR v_float, float* destination) noexcept;
void SIMD_CALLCONV StoreFloat2Aligend(FCVECTOR v_float, Float2A* destination) noexcept;
void SIMD_CALLCONV StoreFloat3(FCVECTOR v_float, float* destination) noexcept;
void SIMD_CALLCONV StoreFloat3Aligned(FCVECTOR v_float, Float3A* destination) noexcept;
void SIMD_CALLCONV StoreFloat4(FCVECTOR v_float, float* destination) noexcept;
void SIMD_CALLCONV StoreFloat4Aligned(FCVECTOR v_float, Float4A* destination) noexcept;

// Ints
void SIMD_CALLCONV StoreUInt4Aligend(FCVECTOR v_int, UInt4A* destination) noexcept;

//******************************************************************************
//*                     Gerneral operations                                    *
//******************************************************************************

VECTOR SIMD_CALLCONV VectorZeros() noexcept;
VECTOR SIMD_CALLCONV VectorFull(float fill_value) noexcept;
VECTOR SIMD_CALLCONV VectorSet(float x, float y, float z, float w) noexcept;
VECTOR SIMD_CALLCONV VectorTrueInt() noexcept;
VECTOR SIMD_CALLCONV VectorFalseInt() noexcept;
VECTOR SIMD_CALLCONV VectorSetInt(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t) noexcept;

float SIMD_CALLCONV VectorGetByIndex(FCVECTOR v, std::size_t i) noexcept;
float SIMD_CALLCONV VectorGetX(FCVECTOR v) noexcept;
float SIMD_CALLCONV VectorGetY(FCVECTOR v) noexcept;
float SIMD_CALLCONV VectorGetZ(FCVECTOR v) noexcept;
float SIMD_CALLCONV VectorGetW(FCVECTOR v) noexcept;

std::uint32_t SIMD_CALLCONV VectorGetIntByIndex(FCVECTOR v, std::size_t i) noexcept;
std::uint32_t SIMD_CALLCONV VectorGetIntX(FCVECTOR v) noexcept;
std::uint32_t SIMD_CALLCONV VectorGetIntY(FCVECTOR v) noexcept;
std::uint32_t SIMD_CALLCONV VectorGetIntZ(FCVECTOR v) noexcept;
std::uint32_t SIMD_CALLCONV VectorGetIntW(FCVECTOR v) noexcept;

VECTOR SIMD_CALLCONV VectorSetByIndex(FCVECTOR v, float f, std::size_t i) noexcept;
VECTOR SIMD_CALLCONV VectorSetX(FCVECTOR v, float x) noexcept;
VECTOR SIMD_CALLCONV VectorSetY(FCVECTOR v, float y) noexcept;
VECTOR SIMD_CALLCONV VectorSetZ(FCVECTOR v, float z) noexcept;
VECTOR SIMD_CALLCONV VectorSetW(FCVECTOR v, float w) noexcept;

VECTOR SIMD_CALLCONV VectorSetIntByIndex(FCVECTOR v, uint32_t x, size_t i) noexcept;
VECTOR SIMD_CALLCONV VectorSetIntX(FCVECTOR v, uint32_t x) noexcept;
VECTOR SIMD_CALLCONV VectorSetIntY(FCVECTOR v, uint32_t y) noexcept;
VECTOR SIMD_CALLCONV VectorSetIntZ(FCVECTOR v, uint32_t z) noexcept;
VECTOR SIMD_CALLCONV VectorSetIntW(FCVECTOR v, uint32_t w) noexcept;

VECTOR SIMD_CALLCONV VectorReplicateX(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorReplicateY(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorReplicateZ(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorReplicateW(FCVECTOR v) noexcept;

VECTOR SIMD_CALLCONV VectorCompose(FCVECTOR v1, FCVECTOR v2, std::uint32_t i0, std::uint32_t i1, std::uint32_t i2,
                                   std::uint32_t i3) noexcept;
template <std::uint32_t i0, std::uint32_t i1, std::uint32_t i2, std::uint32_t i3>
VECTOR SIMD_CALLCONV VectorCompose(FCVECTOR v1, FCVECTOR v2) noexcept;

// selection either 0 or 1
VECTOR SIMD_CALLCONV VectorSetMask(uint32_t select_1, uint32_t select_2, uint32_t select_3, uint32_t select_4) noexcept;

// returns (v[i0],v[i1],v[i2],v[i3])
VECTOR SIMD_CALLCONV VectorReorder(FCVECTOR v, uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3) noexcept;
template <std::uint32_t i0, std::uint32_t i1, std::uint32_t i2, std::uint32_t i3>
VECTOR SIMD_CALLCONV VectorReorder(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorSelect(FCVECTOR v1, FCVECTOR v2, FCVECTOR mask) noexcept;
// returns (v1[0],v1[1],v2[0],v2[1])
VECTOR SIMD_CALLCONV VectorMergeXY(FCVECTOR v1, FCVECTOR v2) noexcept;
// returns (v1[2],v1[3],v2[2],v2[3])
VECTOR SIMD_CALLCONV VectorMergeZW(FCVECTOR v1, FCVECTOR v2) noexcept;

VECTOR SIMD_CALLCONV VectorRotateLeft(FCVECTOR v, uint32_t elements) noexcept;
VECTOR SIMD_CALLCONV VectorRotateRight(FCVECTOR v, uint32_t elements) noexcept;

VECTOR SIMD_CALLCONV VectorEqual(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorEqualInt(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorNearEqual(FCVECTOR v1, FCVECTOR v2, FCVECTOR epsilon) noexcept;
VECTOR SIMD_CALLCONV VectorNotEqual(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorNotEqualInt(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 > v2
VECTOR SIMD_CALLCONV VectorGreater(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 >= v2
VECTOR SIMD_CALLCONV VectorGreaterOrEqual(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 < v2
VECTOR SIMD_CALLCONV VectorLess(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 <= v2
VECTOR SIMD_CALLCONV VectorLessOrEqual(FCVECTOR v1, FCVECTOR v2) noexcept;
// -bounds <= v <= bounds
VECTOR SIMD_CALLCONV VectorInBounds(FCVECTOR v, FCVECTOR bounds) noexcept;

VECTOR SIMD_CALLCONV VectorIsNaN(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorIsInfinite(FCVECTOR v) noexcept;

VECTOR SIMD_CALLCONV VectorMin(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorMax(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorRound(FCVECTOR v) noexcept;
// like floor for positive Numbers and like ceil for negative Numbers
VECTOR SIMD_CALLCONV VectorTruncate(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorFloor(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorCeil(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorClamp(FCVECTOR v, FCVECTOR min, FCVECTOR max) noexcept;
// Pseudocode
// VECTOR Result;
//
// Result.x = min(max(V1.x, 0.0f), 1.0f);
// Result.y = min(max(V1.y, 0.0f), 1.0f);
// Result.z = min(max(V1.z, 0.0f), 1.0f);
// Result.w = min(max(V1.w, 0.0f), 1.0f);
//
// return Result;
VECTOR SIMD_CALLCONV VectorSaturate(FCVECTOR v) noexcept;

// Logical
// ~v1
VECTOR SIMD_CALLCONV VectorNotInt(FCVECTOR v1) noexcept;
// v1 & v2
VECTOR SIMD_CALLCONV VectorAndInt(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 & ~ v2
VECTOR SIMD_CALLCONV VectorAndNotInt(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 | v2
VECTOR SIMD_CALLCONV VectorOrInt(FCVECTOR v1, FCVECTOR v2) noexcept;
// ~(v1 | v2)
VECTOR SIMD_CALLCONV VectorNorInt(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 ^ v2
VECTOR SIMD_CALLCONV VectorXorInt(FCVECTOR v1, FCVECTOR v2) noexcept;

// -v
VECTOR SIMD_CALLCONV VectorNegate(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorAdd(FCVECTOR v1, FCVECTOR v2) noexcept;
// VECTOR Result
// Result[0] = Result[1] = Result[2] = Result[3] = v[0]+v[1]+v[2]+v[3]
VECTOR SIMD_CALLCONV VectorSum(FCVECTOR v) noexcept;
// Angles in radians
VECTOR SIMD_CALLCONV VectorSubtract(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorMultiply(FCVECTOR v1, FCVECTOR v2) noexcept;
// v1 * v2 + v3
VECTOR SIMD_CALLCONV VectorMultiplyAdd(FCVECTOR v1, FCVECTOR v2, FCVECTOR V3) noexcept;
VECTOR SIMD_CALLCONV VectorDivide(FCVECTOR v1, FCVECTOR v2) noexcept;
// v3 - v1 * v2
VECTOR SIMD_CALLCONV VectorNegativeMultiplySubtract(FCVECTOR v1, FCVECTOR v2, FCVECTOR V3) noexcept;
VECTOR SIMD_CALLCONV VectorScale(FCVECTOR v, float factor) noexcept;
// 1/v
VECTOR SIMD_CALLCONV VectorInverseEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorInverse(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorSqrtEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorSqrt(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorInverseSqrtEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorInverseSqrt(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorExp2(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorExpE(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorLog2(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorLogE(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorPow(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorAbs(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorMod(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorSin(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorSinEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorCos(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorCosEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorTan(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorTanEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorASin(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorASinEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorACos(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorACosEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorATan(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorATanEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorATan2(FCVECTOR y, FCVECTOR x) noexcept;
VECTOR SIMD_CALLCONV VectorATan2Est(FCVECTOR y, FCVECTOR x) noexcept;
// Interpolation
// Linearinterpolate
// v0 + t * (v1 - v0)
VECTOR SIMD_CALLCONV VectorLerp(FCVECTOR v0, FCVECTOR v1, float t) noexcept;
VECTOR SIMD_CALLCONV VectorLerpV(FCVECTOR v0, FCVECTOR v1, FCVECTOR v_t) noexcept;

//******************************************************************************
//*                             Lineare algebra                                *
//******************************************************************************
// Vector3
VECTOR SIMD_CALLCONV Vector3Full(float fill_value) noexcept;
VECTOR SIMD_CALLCONV Vector3Zeros() noexcept;
VECTOR SIMD_CALLCONV Vector3Orthogonal(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV Vector3Cross(FCVECTOR v1, FCVECTOR v2, FCVECTOR V3) noexcept;
VECTOR SIMD_CALLCONV Vector3Transform(FCMATRIX m, HCVECTOR v) noexcept;

// Vector
VECTOR SIMD_CALLCONV VectorDot(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorLengthSq(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorInverseLengthEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorInverseLength(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorLengthEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorLength(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorNormalizeEst(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorNormalize(FCVECTOR v) noexcept;
// result = incident - 2*dot(incident,normal)*normal
VECTOR SIMD_CALLCONV VectorReflect(FCVECTOR incident, FCVECTOR normal) noexcept;
VECTOR SIMD_CALLCONV VectorAngleBetweenNormalsEst(FCVECTOR n1, FCVECTOR n2) noexcept;
VECTOR SIMD_CALLCONV VectorAngleBetweenNormals(FCVECTOR n1, FCVECTOR n2) noexcept;
VECTOR SIMD_CALLCONV VectorAngleBetweenVectors(FCVECTOR v1, FCVECTOR v2) noexcept;

// Matrix
bool SIMD_CALLCONV MatrixIsNaN(FCMATRIX m) noexcept;
bool SIMD_CALLCONV MatrixIsInfinite(FCMATRIX m) noexcept;
MATRIX SIMD_CALLCONV MatrixIdentity() noexcept;
MATRIX SIMD_CALLCONV MatrixSet(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13,
                               float m20, float m21, float m22, float m23, float m30, float m31, float m32,
                               float m33) noexcept;
MATRIX SIMD_CALLCONV Matrix3x3Set(float m00, float m01, float m02, float m10, float m11, float m12, float m20,
                                  float m21, float m22);

VECTOR SIMD_CALLCONV MatrixDotVector(FCMATRIX m, HCVECTOR v) noexcept;
MATRIX SIMD_CALLCONV MatrixDotMatrix(FCMATRIX m1, GCMATRIX m2) noexcept;
MATRIX SIMD_CALLCONV MatrixDotMatrixTranspose(FCMATRIX m1, GCMATRIX m2) noexcept;
MATRIX SIMD_CALLCONV MatrixTranspose(FCMATRIX m) noexcept;
MATRIX SIMD_CALLCONV TransformationInverse(FCMATRIX m) noexcept;
// v1*v2^T
MATRIX SIMD_CALLCONV MatrixVectorTensorProduct(FCVECTOR v1, FCVECTOR v2) noexcept;

MATRIX SIMD_CALLCONV MatrixTranslation(float offset_x, float offset_y, float offset_z) noexcept;
MATRIX SIMD_CALLCONV MatrixTranslationFromVector(FCVECTOR offset) noexcept;
MATRIX SIMD_CALLCONV MatrixScaling(float scale_x, float scale_y, float scale_z) noexcept;
MATRIX SIMD_CALLCONV MatrixScalingFromVector(FCVECTOR scale) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationX(float angle) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationY(float angle) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationZ(float angle) noexcept;
MATRIX SIMD_CALLCONV MatrixRotation(float angle_x, float angle_y, float angle_z) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationFromVector(FCVECTOR angles) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationScaleTranslation(FCVECTOR rotation, FCVECTOR scale, FCVECTOR offset);
MATRIX SIMD_CALLCONV MatrixRotationNormal(FCVECTOR normal_axis) noexcept;
//******************************************************************************
//*                              Globals                                       *
//******************************************************************************
struct alignas(DataTypeAligment) VECTORUINT
{
    union {
        std::uint32_t u[4];
        VECTOR v;
    };

    inline operator VECTOR() const noexcept
    {
        return v;
    }
#ifdef _SIMD_SSE_AVAILABLE
    inline operator __m128i() const noexcept
    {
        return _mm_castps_si128(v);
    }
    inline operator __m128d() const noexcept
    {
        return _mm_castps_pd(v);
    }
#endif
};
constexpr std::uint32_t G_SELECT = 0xFFFFFFFF;
constexpr std::uint32_t G_UNSELECT = 0x00000000;
constexpr VECTORUINT G_MASK0000 = {G_UNSELECT, G_UNSELECT, G_UNSELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK0001 = {G_UNSELECT, G_UNSELECT, G_UNSELECT, G_SELECT};
constexpr VECTORUINT G_MASK0010 = {G_UNSELECT, G_UNSELECT, G_SELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK0011 = {G_UNSELECT, G_UNSELECT, G_SELECT, G_SELECT};
constexpr VECTORUINT G_MASK0100 = {G_UNSELECT, G_SELECT, G_UNSELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK0101 = {G_UNSELECT, G_SELECT, G_UNSELECT, G_SELECT};
constexpr VECTORUINT G_MASK0110 = {G_UNSELECT, G_SELECT, G_SELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK0111 = {G_UNSELECT, G_SELECT, G_SELECT, G_SELECT};
constexpr VECTORUINT G_MASK1000 = {G_SELECT, G_UNSELECT, G_UNSELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK1001 = {G_SELECT, G_UNSELECT, G_UNSELECT, G_SELECT};
constexpr VECTORUINT G_MASK1010 = {G_SELECT, G_UNSELECT, G_SELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK1011 = {G_SELECT, G_UNSELECT, G_SELECT, G_SELECT};
constexpr VECTORUINT G_MASK1100 = {G_SELECT, G_SELECT, G_UNSELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK1101 = {G_SELECT, G_SELECT, G_UNSELECT, G_SELECT};
constexpr VECTORUINT G_MASK1110 = {G_SELECT, G_SELECT, G_SELECT, G_UNSELECT};
constexpr VECTORUINT G_MASK1111 = {G_SELECT, G_SELECT, G_SELECT, G_SELECT};

constexpr std::uint32_t G_X_IDX = 0;
constexpr std::uint32_t G_Y_IDX = 1;
constexpr std::uint32_t G_Z_IDX = 2;
constexpr std::uint32_t G_W_IDX = 3;

constexpr std::uint32_t G_COMPOSE_0X = 0;
constexpr std::uint32_t G_COMPOSE_0Y = 1;
constexpr std::uint32_t G_COMPOSE_0Z = 2;
constexpr std::uint32_t G_COMPOSE_0W = 3;
constexpr std::uint32_t G_COMPOSE_1X = 4;
constexpr std::uint32_t G_COMPOSE_1Y = 5;
constexpr std::uint32_t G_COMPOSE_1Z = 6;
constexpr std::uint32_t G_COMPOSE_1W = 7;

constexpr VECTOR G_NEGATIVE_ONE{-1.f, -1.f, -1.f, -1.f};
constexpr VECTOR G_ONE{1.f, 1.f, 1.f, 1.f};
//******************************************************************************
//*                              TODO                                          *
//******************************************************************************
/*
VECTOR SIMD_CALLCONV VectorSubtractAngles(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorAddAngles(FCVECTOR v1, FCVECTOR v2) noexcept;
VECTOR SIMD_CALLCONV VectorModAngles(FCVECTOR Angles) noexcept;

VECTOR SIMD_CALLCONV VectorClampLength(FCVECTOR v, float length_min, float length_max) noexcept;
VECTOR SIMD_CALLCONV VectorClampLengthV(FCVECTOR v, FCVECTOR length_min, FCVECTOR length_max) noexcept;

// Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) +
// sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))
VECTOR SIMD_CALLCONV VectorRefract(FCVECTOR incident, FCVECTOR normal, float refraction_index) noexcept;
VECTOR SIMD_CALLCONV VectorRefractV(FCVECTOR incident, FCVECTOR normal, FCVECTOR refraction_index) noexcept;

VECTOR SIMD_CALLCONV VectorExp10(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorLog10(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorSinH(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorCosH(FCVECTOR v) noexcept;
VECTOR SIMD_CALLCONV VectorTanH(FCVECTOR v) noexcept;
void SIMD_CALLCONV VectorSinCos(VECTOR* sin, FCVECTOR* cos, FCVECTOR v) noexcept;
void SIMD_CALLCONV VectorSinCosEst(VECTOR* sin, FCVECTOR* cos, FCVECTOR v) noexcept;
// Spline Interpolation
// Result = ( 2 * t^3 - 3 * t^2 + 1) * position_0
//        + (     t^3 - 2 * t^2 + t) * tangent_0
//        + (-2 * t^3 + 3 * t^2)     * position_1
//        + (     t^3 -     t^2)     * tangent_1
VECTOR SIMD_CALLCONV VectorHermite(FCVECTOR position_0, FCVECTOR tangent_0, FCVECTOR position_1, FCVECTOR tangent_1,
                                   float t) noexcept;
VECTOR SIMD_CALLCONV VectorHermiteV(FCVECTOR position_0, FCVECTOR tangent_0, FCVECTOR position_1, FCVECTOR tangent_1,
                                    FCVECTOR v_t) noexcept;

// Interpolation
// Pseudocode
// t2 = t * t;
// t3 = t2* t;
//
// P0 = -t3 + 2.0f * t2 - t;
// P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
// P2 = -3.0f * t3 + 4.0f * t2 + t;
// P3 = t3 - t2;
//
// Result.x = (P0 * position_0.x + P1 * position_1.x + P2 * Position2.x + P3 * Position3.x) / 2;
// Result.y = (P0 * position_0.y + P1 * position_1.y + P2 * Position2.y + P3 * Position3.y) / 2;
// Result.z = (P0 * position_0.z + P1 * position_1.z + P2 * Position2.z + P3 * Position3.z) / 2;
// Result.w = (P0 * position_0.w + P1 * position_1.w + P2 * Position2.w + P3 * Position3.w) / 2;
VECTOR SIMD_CALLCONV VectorCatmullRom(FCVECTOR position_0, FCVECTOR position_1, FCVECTOR Position2, FCVECTOR Position3,
                                      float t) noexcept;
VECTOR SIMD_CALLCONV VectorCatmullRomV(FCVECTOR position_0, FCVECTOR position_1, FCVECTOR Position2, FCVECTOR Position3,
                                       FCVECTOR v_t) noexcept;

VECTOR SIMD_CALLCONV MatrixDeterminant(FCMATRIX m) noexcept;
bool SIMD_CALLCONV MatrixIsIdentity(FCMATRIX m) noexcept;
MATRIX SIMD_CALLCONV MatrixAffineTransformation2D(FCVECTOR Scaling, FCVECTOR RotationOrigin, float Rotation,
                                                  FCVECTOR Translation) noexcept;
MATRIX SIMD_CALLCONV MatrixTransformation2D(FCVECTOR ScalingOrigin, float ScalingOrientation, FCVECTOR Scaling,
                                            FCVECTOR RotationOrigin, float Rotation, GXMVECTOR Translation) noexcept;
MATRIX SIMD_CALLCONV MatrixReflect(FCVECTOR ReflectionPlane) noexcept;
MATRIX SIMD_CALLCONV MatrixShadow(FCVECTOR ShadowPlane, FCVECTOR LightPosition) noexcept;
MATRIX SIMD_CALLCONV MatrixLookAtLH(FCVECTOR EyePosition, FCVECTOR FocusPosition, FCVECTOR UpDirection) noexcept;
MATRIX SIMD_CALLCONV MatrixLookAtRH(FCVECTOR EyePosition, FCVECTOR FocusPosition, FCVECTOR UpDirection) noexcept;
MATRIX SIMD_CALLCONV MatrixLookToLH(FCVECTOR EyePosition, FCVECTOR EyeDirection, FCVECTOR UpDirection) noexcept;
MATRIX SIMD_CALLCONV MatrixLookToRH(FCVECTOR EyePosition, FCVECTOR EyeDirection, FCVECTOR UpDirection) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveFovRH(float FovAngleY, float AspectRatio, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveOffCenterLH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop,
                                                  float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixPerspectiveOffCenterRH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop,
                                                  float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixOrthographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixOrthographicRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixOrthographicOffCenterLH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop,
                                                   float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixOrthographicOffCenterRH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop,
                                                   float NearZ, float FarZ) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationQuaternion(FCVECTOR Quaternion) noexcept;
bool SIMD_CALLCONV MatrixDecompose(VECTOR* outScale, VECTOR* outRotQuat, VECTOR* outTrans, FCMATRIX m) noexcept;
MATRIX SIMD_CALLCONV MatrixTransformation(FCVECTOR ScalingOrigin, FCVECTOR ScalingOrientationQuaternion,
                                          FCVECTOR Scaling, GCVECTOR RotationOrigin, HCVECTOR RotationQuaternion,
                                          HCVECTOR Translation) noexcept;
MATRIX SIMD_CALLCONV MatrixAffineTransformation(FCVECTOR Scaling, FCVECTOR RotationOrigin, FCVECTOR RotationQuaternion,
                                                GCVECTOR Translation) noexcept;
//******************************************************************************
//*                     Conversion operations                                  *
//******************************************************************************
VECTOR SIMD_CALLCONV VectorConvertSIntToFloat(FCVECTOR v_int) noexcept;
VECTOR SIMD_CALLCONV VectorConvertFloatToSInt(FCVECTOR v_float) noexcept;
VECTOR SIMD_CALLCONV VectorConvertUIntToFloat(FCVECTOR v_uint) noexcept;
VECTOR SIMD_CALLCONV VectorConvertFloatToUInt(FCVECTOR v_float) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationRollPitchYaw(float Pitch, float Yaw, float Roll) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationRollPitchYawFromVector(FCVECTOR Angles) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationNormal(FCVECTOR normal_axis, float angle) noexcept;
MATRIX SIMD_CALLCONV MatrixRotationAxis(FCVECTOR axis, float angle) noexcept;
*/
} // namespace simd
} // namespace jpctracer