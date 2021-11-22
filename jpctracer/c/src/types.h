#pragma once
#include <cglm/cglm.h>

typedef struct
{
    vec3 origin;
    vec3 direction;
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
    float pdf;
    vec4  color;
} sampled_color_t;


