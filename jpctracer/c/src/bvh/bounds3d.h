#pragma once
#include "../types.h"
#include <cglm/cglm.h>

typedef struct
{
    vec3 origin;
    vec3 direction;
} ray_trav_t;

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