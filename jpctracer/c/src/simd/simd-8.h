#pragma once
#if (SIMD_WIDTH == 8)
#include <immintrin.h> //AVX
/*
#ifndef __FMA__
#error FMA required
#endif

#ifndef __AVX2__
#error AVC2 required
#endif
*/

#include <immintrin.h>

typedef struct
{
    union {
        __m256 m;
        float  data[8];
    };
} simdf;

// a*b-c
static inline simdf simd_fmsub_ps(simdf a, simdf b, simdf c)
{
    return (simdf){_mm256_fmsub_ps(a.m, b.m, c.m)};
}

static inline simdf m_min_nan4(simdf a, simdf b)
{
    __m256i tmp_a = _mm256_castps_si256(a.m);
    __m256i tmp_b = _mm256_castps_si256(b.m);
    __m256i tmp_result = _mm256_min_epi32(tmp_a, tmp_b);
    return (simdf){_mm256_castsi256_ps(tmp_result)};
}
static inline simdf simd_min4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = m_min_nan4(a, b);
    simdf tmp2 = m_min_nan4(c, d);
    return m_min_nan4(tmp1, tmp2);
}

static inline simdf m_max_nan4(simdf a, simdf b)
{
    __m256i tmp_a = _mm256_castps_si256(a.m);
    __m256i tmp_b = _mm256_castps_si256(b.m);
    __m256i tmp_result = _mm256_max_epi32(tmp_a, tmp_b);
    return (simdf){_mm256_castsi256_ps(tmp_result)};
}

static inline simdf simd_max4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = m_max_nan4(a, b);
    simdf tmp2 = m_max_nan4(c, d);
    return m_max_nan4(tmp1, tmp2);
}

static inline simdf simd_set_ps1(float a)
{
    return (simdf){_mm256_set1_ps(a)};
}

static inline int simd_cmple_ps(simdf a, simdf b)
{

    __m256 mask = _mm256_cmp_ps(a.m, b.m, _CMP_LE_OQ);
    return _mm256_movemask_ps(mask);
}

#endif