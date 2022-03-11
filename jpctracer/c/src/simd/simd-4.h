#pragma once
#define SIMD_WIDTH 4
#if (SIMD_WIDTH == 4)
#include <emmintrin.h> //SSE2
#include <xmmintrin.h> //SSE
#ifdef __FMA__
#include <immintrin.h>
#endif

typedef struct
{
    __m128 m;
} simdf;

// a*b-c
static inline simdf simd_fmsub_ps(simdf a, simdf b, simdf c)
{
#ifdef __FMA__
    return (simdf){_mm_fmsub_ps(a.m, b.m, c.m)};
#else
    return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif
}

static inline simdf m_min_nan4(simdf a, simdf b)
{
    __m128i tmp_a = _mm_castps_si128(a.m);
    __m128i tmp_b = _mm_castps_si128(b.m);
    __m128i tmp_result = _mm_min_epi32(tmp_a, tmp_b);
    return (simdf){_mm_castsi128_ps(tmp_result)};
}
static inline simdf simd_min4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = m_min_nan4(a, b);
    simdf tmp2 = m_min_nan4(c, d);
    return m_min_nan4(tmp1, tmp2);
}

static inline simdf m_max_nan4(simdf a, simdf b)
{
    __m128i tmp_a = _mm_castps_si128(a.m);
    __m128i tmp_b = _mm_castps_si128(b.m);
    __m128i tmp_result = _mm_max_epi32(tmp_a, tmp_b);
    return (simdf){_mm_castsi128_ps(tmp_result)};
}

static inline simdf simd_max4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = m_max_nan4(a, b);
    simdf tmp2 = m_max_nan4(c, d);
    return m_max_nan4(tmp1, tmp2);
}

static inline simdf simd_set_ps1(float a)
{
    return (simdf){_mm_set_ps1(a)};
}

static inline int simd_cmple_ps(simdf a, simdf b)
{

    __m128 mask = _mm_cmple_ps(a.m, b.m);
    return _mm_movemask_ps(mask);
}

#endif