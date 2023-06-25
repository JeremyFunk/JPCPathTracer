#pragma once
#include "../types.h"
#include <cglm/cglm.h>


typedef struct
{
    float min;
    float max;
} intervall_t;


typedef struct
{
    vec3  org; // org * inv_dir
    vec3  inv_dir;
    int   load_permul_min[3];
    int   load_permul_max[3];
    float clip_end;
} ray_trav_bounds_t;

static inline ray_trav_bounds_t ray_trav_bounds_make(const ray_t* ray)
{
    ray_trav_bounds_t result;
    for (int i = 0; i < 3; i++)
    {
        result.inv_dir[i] = (float) 1. / ray->direction[i];
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
    result.clip_end = ray->clip_end;
    return result;
}
