#pragma once
#include "../geom_log.h"
#include "shapes.h"
#include <immintrin.h>
#include <xmmintrin.h>

typedef struct
{
    __m128 inv_dir_org[3]; // org * inv_dir
    __m128 inv_dir[3];
    int    load_permul_min[3];
    int    load_permul_max[3];
    __m128 t_min;
    __m128 t_max;
} ray_trav_bounds4_t;

typedef struct
{
    vec3  org; // org * inv_dir
    vec3  inv_dir;
    int   load_permul_min[3];
    int   load_permul_max[3];
    float t_min;
    float t_max;
} ray_trav_bounds_t;

static inline ray_trav_bounds_t ray_trav_bounds_make(const ray_trav_t* ray,
                                                     float             t_min,
                                                     float             t_max)
{
    ray_trav_bounds_t result;
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir[i] = 1. / ray->direction[i];
    }

    for (int i = 0; i < 3; i++)
    {
        result.org[i] = ray->origin[i];
    }
    for (int i = 0; i < 3; i++)
    {
        if (ray->direction[i] < 0)
        {
            result.load_permul_min[i] = i * 2 + 1;
            result.load_permul_max[i] = i * 2;
        }
        else
        {
            result.load_permul_min[i] = i * 2;
            result.load_permul_max[i] = i * 2 + 1;
        }
    }
    result.t_max = t_max;
    result.t_min = t_min;
    return result;
}

static inline ray_trav_bounds4_t ray_trav_bounds4_make(
    const ray_trav_bounds_t* ray,
    float                    t_min,
    float                    t_max)
{
    ray_trav_bounds4_t result;
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir[i] = _mm_set_ps1(ray->inv_dir[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir_org[i] = _mm_set_ps1(ray->inv_dir[i] * ray->org[i]);
    }
    for (int i = 0; i < 3; i++)
        result.load_permul_min[i] = ray->load_permul_min[i];
    for (int i = 0; i < 3; i++)
        result.load_permul_max[i] = ray->load_permul_max[i];
    result.t_min = _mm_set_ps1(t_min);
    result.t_max = _mm_set_ps1(t_max);
    return result;
}

typedef struct bounds4_s
{
    // Layout:
    // min_x, max_x, min_y, max_y, min_z, max_z
    __m128 borders[6];
} bounds4_t;

typedef struct hits_bounds4_s
{
    __m128 min;
    __m128 max;
    int    mask;

} hits_bounds4_t;

static inline __m128 min_nan4(__m128 a, __m128 b)
{
    __m128i tmp_a = _mm_castps_si128(a);
    __m128i tmp_b = _mm_castps_si128(b);
    __m128i tmp_result = _mm_min_epi32(tmp_a, tmp_b);
    return _mm_castsi128_ps(tmp_result);
}
static inline __m128 min4(__m128 a, __m128 b, __m128 c, __m128 d)
{
    __m128 tmp1 = min_nan4(a, b);
    __m128 tmp2 = min_nan4(c, d);
    return min_nan4(tmp1, tmp2);
}

static inline __m128 max_nan4(__m128 a, __m128 b)
{
    __m128i tmp_a = _mm_castps_si128(a);
    __m128i tmp_b = _mm_castps_si128(b);
    __m128i tmp_result = _mm_max_epi32(tmp_a, tmp_b);
    return _mm_castsi128_ps(tmp_result);
}

static inline __m128 max4(__m128 a, __m128 b, __m128 c, __m128 d)
{
    __m128 tmp1 = max_nan4(a, b);
    __m128 tmp2 = max_nan4(c, d);
    return max_nan4(tmp1, tmp2);
}

static inline hits_bounds4_t bounds3d_intersect4(const ray_trav_bounds4_t* ray,
                                                 const bounds4_t* bounds)
{
    __m128 mins[3];
    __m128 maxs[3];

    for (int i = 0; i < 3; i++)
    {
        mins[i] = _mm_fmsub_ps(bounds->borders[ray->load_permul_min[i]],
                               ray->inv_dir[i],
                               ray->inv_dir_org[i]); //(bounds.min-org)/dir
    }

    for (int i = 0; i < 3; i++)
    {
        maxs[i] = _mm_fmsub_ps(bounds->borders[ray->load_permul_max[i]],
                               ray->inv_dir[i],
                               ray->inv_dir_org[i]); //(bounds.min-org)/dir
    }
    hits_bounds4_t hits;
    hits.min = max4(mins[0], mins[1], mins[2], ray->t_min);
    hits.max = min4(maxs[0], maxs[1], maxs[2], ray->t_max);
    __m128 mask = _mm_cmple_ps(hits.min, hits.max); // hits.min<=hits.max
    hits.mask = _mm_movemask_ps(mask);
    return hits;
};

static inline void bounds4_log(const char* name, const bounds4_t* b, int i)
{
    vec3 min;
    vec3 max;
    for (int j = 0; j < 3; j++)
    {
        min[j] = b->borders[2 * j][i];
        max[j] = b->borders[2 * j + 1][i];
    }
    LOG_BOUNDS(name, min, max);
}