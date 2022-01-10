#include "scattering.h"
#include "bsdf.h"
#include "bvh.h"
#include "cglm/call/vec3.h"
#include "jpc_api.h"
#include "lights.h"
#include "sampler.h"
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

float cos_weight(vec3 a, vec3 b)
{
    float w = glmc_vec3_dot(a, b);
    // flip direction of a
    //w*=-1;

    return w>0 ? w : 0;
}

void combine_rays(sampled_color_t* bsdf_colors,
                  sampled_color_t* light_colors,
                  vec3*            scatterd_dirs,
                  uint             n,
                  vec3             incident_dir,
                  vec4             result)
{
    glm_vec4_zero(result);

    for (int i = 0; i < n; i++)
    {
        vec4 temp;
        glm_vec4_mul(bsdf_colors[i].color, light_colors[i].color, temp);
        float w = cos_weight(scatterd_dirs[i], incident_dir);
        glm_vec4_scale(temp, w / light_colors->pdf, temp);

        glm_vec4_add(temp, result, result);
        if(result[1]>0.1)
        {
            //printf("test\n");
        }
        for (int j = 0; j < 3; j++)
        {
//            result[j] = w;
            //result[j] = scatterd_dirs[i][j];
            // result[j] = incident_dir[j];
        }
    }
}

typedef struct ray_evaluator_s
{
    const scene_t*   scene;
    sampler_state*   sampler;
    bsdfcontext_t*   bsdf;
    uint             direct_count;
    uint             indirect_count;
    sampled_color_t* colors;
    sampled_color_t* light_colors;
    float*           direct_clip_ends;
    vec3*            directions;

    vec2* samples;
    uint2 direct_samples;
    uint2 indirect_samples;

} ray_evaluator_t;

ray_evaluator_t* ray_evaluator_init(const scene_t* scene,
                                    sampler_state* sampler,
                                    uint           direct_count,
                                    uint           indirect_count)
{

    assert(direct_count < 64);
    uint direct_n_sqrt = sqrt(direct_count);
    uint indirect_n_sqrt = sqrt(indirect_count);

    direct_count = direct_n_sqrt * direct_n_sqrt;
    indirect_count = indirect_n_sqrt * indirect_n_sqrt;
    uint all_count = direct_count + indirect_count;

    ray_evaluator_t* result = malloc(sizeof *result);
    result->scene = scene;
    result->sampler = sampler;
    result->bsdf = bsdf_alloc(bsdf_default_limits);
    result->direct_count = direct_count;
    result->indirect_count = indirect_count;
    result->colors = malloc(all_count * sizeof(*result->colors));
    result->light_colors = malloc(direct_count * sizeof(*result->light_colors));
    result->direct_clip_ends
        = malloc(direct_count * sizeof(*result->direct_clip_ends));
    result->directions = malloc(all_count * sizeof(*result->directions));
    result->samples = malloc(all_count * sizeof(*result->samples));
    result->direct_samples[0] = direct_n_sqrt;
    result->direct_samples[1] = direct_n_sqrt;
    result->indirect_samples[0] = indirect_n_sqrt;
    result->indirect_samples[1] = indirect_n_sqrt;

    return result;
}

void ray_evaluator_free(ray_evaluator_t* evaluator)
{
    bsdf_free(evaluator->bsdf);
    free(evaluator->colors);
    free(evaluator->light_colors);
    free(evaluator->directions);
    free(evaluator->direct_clip_ends);
    free(evaluator->samples);
}

void eval_background(const materiallib_t* matlib,
                     ray_t                incident_ray,
                     vec4                 out_color)
{
    vec4 color = {0, 0, 0,1};
    glm_vec4_copy(color, out_color);
}

// Assumes that result has alloced enougth memory for the indirect rays
void scatter(ray_evaluator_t* eval, ray_t incident_ray, scattering_t* result)
{
    hit_point_t    hitpoint;
    const scene_t* scene = eval->scene;

    if (ray_intersect(&scene->geometries, &incident_ray, &hitpoint))
    {
        sampled_color_t* direct_colors = eval->colors + eval->indirect_count;
        sampled_color_t* indirect_colors = eval->colors;
        vec3*            direct_dirs = eval->directions + eval->indirect_count;
        vec3*            indirect_dirs = eval->directions;

        vec2* direct_samples = eval->samples + eval->indirect_count;
        vec2* indirect_samples = eval->samples;

        // sample lights
        sample2d(eval->sampler, eval->direct_samples, direct_samples);
        sample2d(eval->sampler, eval->indirect_samples, indirect_samples);

        int direct_count = sample_lights(&scene->lights,
                                         indirect_samples,
                                         eval->indirect_count,
                                         hitpoint,
                                         direct_dirs,
                                         eval->direct_clip_ends,
                                         eval->light_colors);

        uint64_t shadow_mask = rays_shadow_test(&scene->geometries,
                                                direct_dirs,
                                                eval->direct_clip_ends,
                                                hitpoint.location,
                                                direct_count);

        for (int i = 0; i < direct_count; i++)
        {
            if (shadow_mask & (1 << i))
            {
                eval->light_colors[i] = (sampled_color_t){
                    .color = {0.f, 0.f, 0.f, 1.f},
                    .pdf = 1,
                };
            }
        }

        result->indirect_count = eval->indirect_count;

        int all_count = eval->indirect_count + direct_count;

        bsdf_init(
            eval->bsdf, &scene->materiallib, incident_ray.direction, hitpoint);

        bsdf_vec3_to_local(eval->bsdf, direct_dirs, direct_count);

        for (int i = 0; i < eval->indirect_count; i++)
            bsdf_sample(eval->bsdf, indirect_samples[i], indirect_dirs + i);
        vec4 emission;
        bsdf_eval(eval->bsdf,
                  eval->directions,
                  all_count,
                  eval->colors,
                  &emission);


        bsdf_vec3_to_world(eval->bsdf, eval->directions, all_count);

        combine_rays(direct_colors,
                     eval->light_colors,
                     direct_dirs,
                     direct_count,
                     incident_ray.direction,
                     result->direct_color);
        //glm_vec4_copy(hitpoint.location,result->direct_color);
        if(result->direct_color[1]>0.6)
        {
//            printf("lol\n");
        }
        result->indirect_count = eval->indirect_count;

        for (int i = 0; i < eval->indirect_count; i++)
        {
            glm_vec3_copy(indirect_dirs[i], result->indirect_rays[i].direction);
            glm_vec3_copy(hitpoint.location, result->indirect_rays[i].origin);
            result->indirect_rays[i].clip_end = scene->camera.clip_end;
            result->indirect_color[i] = indirect_colors[i];
        }
        if (glm_vec3_norm(result->direct_color) > 0.1)
        {
            //            printf("Warn: lol\n");
        }
        // printf("Hit: direct color:");
        // printf_arrayf(4,result->direct_color);
        // printf("\n");
    }
    else
    {
        eval_background(
            &scene->materiallib, incident_ray, result->direct_color);
        result->indirect_count = 0;
    }
}
