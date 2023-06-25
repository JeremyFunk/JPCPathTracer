#pragma once
#include "cglm/types.h"
#include "jpc_api.h"
#include "sampler.h"
#include "types.h"
#include "bsdf.h"

typedef struct
{
    vec4             direct_color;
    uint             indirect_count;
    vec4*             indirect_colors;
    ray_t*           indirect_rays;
    vec3 normal;
} scattering_t;

typedef enum coord_system{
    JPC_COORD_NOTSET,
    JPC_COORD_WORLD,
    JPC_COORD_LOCAL,
}coord_system_t;

typedef struct{
    vec4 emission;

    // Memory layout: [delta lights, sampled lights]
    sampled_color_t* light_colors;

    // Memory layout: [indirect, delta lights, sampled lights]
    sampled_color_t* bsdf_colors;

    // Memory layout: [indirect, delta lights, sampled lights]
    vec3* ray_directions;

    // Memory layout: [delta lights, sampled lights]
    float* distances;

    uint indirect_rays_count;
    uint light_delta_count;
    uint lights_sampled_count;

    

    coord_system_t light_system;
    coord_system_t indirect_system;
}scattering_rays_t;



typedef struct ray_evaluator_s
{
    const scene_t*   scene;
    sampler_state*   sampler;
    bsdfcontext_t   bsdf;

    scattering_rays_t rays;

    uint max_direct_samples_count;
    vec2* direct_samples;

    uint indirect_samples_count;
    vec2* indirect_samples;

} ray_evaluator_t;


/*

        \    |      A
         \   |     /
          \  |   /
           v | /
-------------------------------- Surface

v and A denote the directions of a and b

*/
float cos_weight(vec3 a, vec3 b);



ray_evaluator_t ray_evaluator_init(arena_t* arena,
                                    const scene_t* scene,
                                    sampler_state* sampler,
                                    uint           max_direct_samples,
                                    uint           indirect_count);

void scatter(ray_evaluator_t* evaluator,
             ray_t            incident_ray,
             scattering_t*    result);
