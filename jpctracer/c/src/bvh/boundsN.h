#pragma once

//#define SIMD_WIDTH 2

#ifndef SIMD_WIDTH
#error SIMD_WIDTH musst be defined
#endif

#include "../geom_log.h"
#include "../simd/simd.h"
#include "bounds3d.h"
#include "shapes.h"

typedef struct
{
    simdf inv_dir_org[3]; // org * inv_dir
    simdf inv_dir[3];
    int   load_permul_min[3];
    int   load_permul_max[3];
    simdf t_min;
    simdf t_max;
} ray_trav_boundsN_t;

static inline ray_trav_boundsN_t ray_trav_boundsN_make(
    const ray_trav_bounds_t* ray,
    float                    t_min,
    float                    t_max)
{
    ray_trav_boundsN_t result;
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir[i] = simd_set_ps1(ray->inv_dir[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir_org[i] = simd_set_ps1(ray->inv_dir[i] * ray->org[i]);
    }
    for (int i = 0; i < 3; i++)
        result.load_permul_min[i] = ray->load_permul_min[i];
    for (int i = 0; i < 3; i++)
        result.load_permul_max[i] = ray->load_permul_max[i];
    result.t_min = simd_set_ps1(t_min);
    result.t_max = simd_set_ps1(t_max);
    return result;
}

typedef struct bounds4_s
{
    // Layout:
    // min_x, max_x, min_y, max_y, min_z, max_z
    simdf borders[6];
} boundsN_t;

typedef struct hits_boundsN_s
{
    simdf min;
    simdf max;
    int   mask;

} hits_boundsN_t;

static inline hits_boundsN_t bounds3d_intersectN(const ray_trav_boundsN_t* ray,
                                                 const boundsN_t* bounds)
{
    simdf mins[3];
    simdf maxs[3];

    for (int i = 0; i < 3; i++)
    {
        mins[i] = simd_fmsub_ps(bounds->borders[ray->load_permul_min[i]],
                                ray->inv_dir[i],
                                ray->inv_dir_org[i]); //(bounds.min-org)/dir
    }

    for (int i = 0; i < 3; i++)
    {
        maxs[i] = simd_fmsub_ps(bounds->borders[ray->load_permul_max[i]],
                                ray->inv_dir[i],
                                ray->inv_dir_org[i]); //(bounds.min-org)/dir
    }
    hits_boundsN_t hits;
    hits.min = simd_max4(mins[0], mins[1], mins[2], ray->t_min);
    hits.max = simd_min4(maxs[0], maxs[1], maxs[2], ray->t_max);
    hits.mask = simd_cmple_ps(hits.min, hits.max); // hits.min<=hits.max
    return hits;
}

static inline void bounds4_log(const char* name, const boundsN_t* b, int i)
{
    vec3 min;
    vec3 max;
    for (int j = 0; j < 3; j++)
    {
        min[j] = b->borders[2 * j].m[i];
        max[j] = b->borders[2 * j + 1].m[i];
    }
    LOG_BOUNDS(name, min, max);
}
