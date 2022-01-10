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

typedef vec4 spectrum;

typedef struct
{
    vec4 location;
    int  material_id;
    vec2 uvs;
    vec3 normal;
} hit_point_t;
typedef struct
{
    vec4 color;
    union {
        float pdf;
        vec4  _; //alligment
    };
} sampled_color_t;

typedef struct
{
    vec2 min;
    vec2 max;
} bounds2d_t;
