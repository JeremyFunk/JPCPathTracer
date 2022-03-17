#pragma once
#include "jpc_api.h"
#include <cglm/cglm.h>

typedef struct
{
    vec3 origin;
    vec3 direction;
    // max distance of intersection
    float clip_end;
} ray_t;

static inline ray_t make_ray(const vec3 origin,const vec3 direction, float clip_end)
{
    ray_t ray;
    glm_vec3_copy((float*)origin,ray.origin);
    glm_vec3_copy((float*)direction,ray.direction);
    ray.clip_end = clip_end;
    return ray;
}

typedef vec4 spectrum;

typedef struct
{
    uint instance_id;
    uint mesh_id; 
    float distance;
    vec4 location;
    int  material_id;
    vec2 uv;
    vec3 normal;
} hit_point_t;

typedef struct
{
    vec4 color;
    union {
        float pdf;
        vec4  _; // alligment
    };
} sampled_color_t;

typedef struct
{
    vec2 min;
    vec2 max;
} bounds2d_t;

typedef struct
{
    vec3 min;
    vec3 max;
} bounds3d_t;
