#include "scattering.h"
#include "bsdf.h"
#include "bvh/traverse.h"
#include "cglm/call/vec3.h"
#include "jpc_api.h"
#include "lights.h"
#include "sampler.h"
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <log/log.h>
#include <math.h>
#include <stdlib.h>

float cos_weight(vec3 in_ray, vec3 out_ray)
{
    float w = glmc_vec3_dot(in_ray, out_ray);
    // flip direction of in_ray
    w *= -1;

    return w > 0 ? w : 0;
}

void combine_rays(sampled_color_t* bsdf_colors,
                  sampled_color_t* light_colors,
                  vec3*            local_scatterd_dirs,
                  uint             n,
                  vec4             result)
{
    glm_vec4_zero(result);

    uint non_delta_count = n;

    vec4 non_delta_color = {0, 0, 0, 0};

    for (uint i = 0; i < n; i++)
    {
        vec4 temp;
        glm_vec4_mul(bsdf_colors[i].color, light_colors[i].color, temp);

        float cos_theta = fabs(local_scatterd_dirs[i][2]);
        // the same as |<normal,local_scatterd_dirs[i]>| because normal =
        // (0,0,1)

        float scaler = cos_theta;
        if (light_colors->pdf < 1e-5)
        {
            // this is a delta function;
            non_delta_count -= 1;

            glm_vec4_scale(temp, scaler, temp);
            glm_vec4_add(temp, result, result);
        }
        else
        {
            scaler /= light_colors->pdf;

            glm_vec4_scale(temp, scaler, temp);
            glm_vec4_add(temp, result, result);
        }
    }
    assert(non_delta_count <= n);
    if (non_delta_count > 0)
    {
        glm_vec4_scale(non_delta_color, 1. / non_delta_count, non_delta_color);
        glm_vec4_add(non_delta_color, result, result);
    }
}

void compute_direct_color(const scattering_rays_t* rays, vec4 result)
{
    assert(rays->light_system == JPC_COORD_LOCAL);
    glm_vec4_zero(result);
    //Delta lights
    for(uint i =0;i<rays->light_delta_count;i++)
    {
        vec4 temp;
        float cos_theta = fabs(rays->ray_directions[rays->indirect_rays_count+i][2]);
        glm_vec4_scale(rays->light_colors[i].data,cos_theta,temp);
        glm_vec4_muladd(temp,rays->bsdf_colors[i+rays->indirect_rays_count].data,result);
    }

    //ToDo support Sampled Lights
}

void compute_indirect_color(const scattering_rays_t* rays, vec4* results)
{
    for(uint i = 0; i<rays->indirect_rays_count;i++)
    {

        float cos_theta = fabs(rays->ray_directions[i][2]);
        float scaler = cos_theta/ rays->bsdf_colors[i].pdf;
        glm_vec3_scale(rays->bsdf_colors[i].color,scaler,results[i]);
    }
}

ray_evaluator_t ray_evaluator_init(arena_t*       arena,
                                   const scene_t* scene,
                                   sampler_state* sampler,
                                   uint           max_direct_samples,
                                   uint           indirect_count)
{
    return (ray_evaluator_t){
        .scene = scene,
        .sampler = sampler,
        .bsdf = bsdf_alloc(arena, bsdf_default_limits),
        .rays = scattering_rays_alloc(
            arena, &scene->lights, indirect_count, max_direct_samples),
        .max_direct_samples_count = max_direct_samples,
        .direct_samples
        = ARENA_ARRAY_ALLOC(arena, vec2, max_direct_samples),
        .indirect_samples_count = indirect_count,
        .indirect_samples = ARENA_ARRAY_ALLOC(arena, vec2, indirect_count),
    };
}

void remove_blocked_direct_rays(const geometries_t* geoms,
                                vec3                origin,
                                scattering_rays_t*  rays)
{
    assert(rays->light_system == JPC_COORD_WORLD);
    assert(rays->indirect_system == JPC_COORD_NOTSET);

    uint free_dir_index = 0;
    uint all_rays = rays->light_delta_count + rays->lights_sampled_count;
    uint light_delta_count = rays->light_delta_count;
    for (uint i = 0; i < all_rays; i++)
    {
        assert(free_dir_index <= i);
        ray_t ray
            = make_ray(origin,
                       rays->ray_directions[rays->indirect_rays_count + i],
                       rays->distances[i]);

        if (!ray_intersect_any(geoms, &ray))
        {
            glm_vec4_copy(rays->light_colors[i].data,
                          rays->light_colors[free_dir_index].data);
            glm_vec3_copy(rays->ray_directions[i],
                          rays->ray_directions[free_dir_index]);
            rays->distances[free_dir_index] = rays->distances[i];
            free_dir_index++;
        }
        else if (i < light_delta_count )
        {
            assert(rays->light_delta_count > 0);
            rays->light_delta_count--;
        }
        else 
        {
            assert(rays->lights_sampled_count > 0);
            rays->lights_sampled_count--;
        }
    }
}

void eval_background(const materiallib_t* matlib,
                     ray_t                incident_ray,
                     vec4                 out_color)
{
    assert(matlib);
    assert(glm_eq(glm_vec3_norm(incident_ray.direction), 1.f));
    vec4 color = {0, 0, 0, 1};
    glm_vec4_copy(color, out_color);
}

void scatter(ray_evaluator_t* eval, ray_t incident_ray, scattering_t* result)
{
    hit_point_t    hitpoint;
    const scene_t* scene = eval->scene;
    scattering_rays_t* rays = &eval->rays;
    scattering_rays_reset(&scene->lights,
    eval->indirect_samples_count,eval->max_direct_samples_count,rays );

    if (ray_intersect_c3(&scene->geometries, &incident_ray, &hitpoint))
    {

        sample2d(eval->sampler,
                 eval->max_direct_samples_count,
                 eval->direct_samples);
        sample2d(eval->sampler,
                 eval->indirect_samples_count,
                 eval->indirect_samples);

        assert(eval->indirect_samples[0][0] < 1. + 1e-6);

        sample_lights(&scene->lights,
                      eval->direct_samples,
                      eval->max_direct_samples_count,
                      hitpoint.location,
                      rays);

        remove_blocked_direct_rays(&scene->geometries,hitpoint.location,rays);

        bsdf_init(
            &eval->bsdf, &scene->materiallib, incident_ray.direction, hitpoint);

        bsdf_vec3_to_local(&eval->bsdf, rays->ray_directions+rays->indirect_rays_count, 
        rays->light_delta_count+rays->lights_sampled_count
        );

        rays->light_system = JPC_COORD_LOCAL;


        for (uint i = 0; i < rays->indirect_rays_count; i++)
        {

            assert(eval->indirect_samples[i][0] < 1. + 1e-6);
            bsdf_sample(&eval->bsdf,eval->indirect_samples[i],rays->ray_directions + i);
        }
        rays->indirect_system = JPC_COORD_LOCAL;

        uint all_count = rays->indirect_rays_count+rays->light_delta_count+rays->lights_sampled_count;
        bsdf_eval(
            &eval->bsdf, rays->ray_directions, all_count, rays->bsdf_colors, &rays->emission);

        compute_direct_color(rays,result->direct_color);
        result->indirect_count = rays->indirect_rays_count;

        compute_indirect_color(rays,result->indirect_colors);

        bsdf_vec3_to_world(&eval->bsdf, rays->ray_directions, rays->indirect_rays_count);
        rays->indirect_system = JPC_COORD_WORLD;

        for (uint i = 0; i < rays->indirect_rays_count; i++)
        {
            glm_vec3_copy(rays->ray_directions[i], result->indirect_rays[i].direction);
            glm_vec3_copy(hitpoint.location, result->indirect_rays[i].origin);
            result->indirect_rays[i].clip_end = scene->camera.clip_end;
        }

        glm_vec3_copy(hitpoint.normal, result->normal);
    }
    else
    {
        eval_background(
            &scene->materiallib, incident_ray, result->direct_color);
        result->indirect_count = 0;
    }
}
