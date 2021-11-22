#pragma once
#include"types.h"

typedef struct {
    uint max_size;
    uint all_size;
    uint indirect_size;
    uint direct_size;
    //origin colors
    sampled_color_t* all_org_colors;
    sampled_color_t* indirect_org_colors;
    sampled_color_t* direct_org_colors;
    //destination colors
    sampled_color_t* direct_dest_colors;

    vec3* all_directions;
    vec3* indirect_directions;
    vec3* direct_directions;
    vec4 emission;
    vec3 incident_dir;

    hit_point_t hitpoint;


} scattering_t;


typedef struct
{
    vec4 indirect_color;
    vec4 direct_color;
    int  max_depth;
    int  depth;
} render_integral_t;

render_integral_t integral_init(int max_depth);

bool integral_next(render_integral_t* integral, const scattering_t* scattering, ray_t* next_ray);

void integral_get_color(render_integral_t* integral, vec4* out_color);
