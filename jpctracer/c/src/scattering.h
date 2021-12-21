#pragma once
#include "cglm/types.h"
#include "jpc_api.h"
#include "sampler.h"
#include "types.h"

typedef struct
{
    vec4             direct_color;
    uint             indirect_count;
    sampled_color_t* indirect_color;
    ray_t*           indirect_rays;
} scattering_t;

typedef struct ray_evaluator_s ray_evaluator_t;

/*

        \    |      A
         \   |     /
          \  |   /
           v | /
-------------------------------- Surface

v and A denote the directions of a and b

*/
float cos_weight(vec3 a, vec3 b);

ray_evaluator_t* ray_evaluator_init(const scene_t* scene,
                                    sampler_state* sampler,
                                    uint           direct_rays,
                                    uint           indirect_rays);

void ray_evaluator_free(ray_evaluator_t* evaluator);

void scatter(ray_evaluator_t* evaluator,
             ray_t            incident_ray,
             scattering_t*    result);