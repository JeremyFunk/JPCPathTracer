#include "integrator.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"
#include "sampler.h"
#include "scattering.h"
#include "types.h"

#include <stdlib.h>

typedef struct integrator_s
{
    ray_evaluator_t* evaluator;
    uint             max_depth;
} integrator_t;

integrator_t* integrator_init(int            max_depth,
                              const scene_t* scene,
                              sampler_state* sampler,
                              uint           light_samples)
{
    integrator_t* result = malloc(sizeof(*result));
    result->evaluator = ray_evaluator_init(scene, sampler, light_samples, 1);
    result->max_depth = max_depth;
    return result;
}

void integrator_free(integrator_t* integrator)
{
    ray_evaluator_free(integrator->evaluator);
}

void integrate(integrator_t* integrator, ray_t ray, vec4 result)
{
    // update scheme
    // L_i = E_i + f_i L_{i+1}
    // L_0 = E_0 + f_0 (E_1 + f_1 (E_2 + f_2 ))
    // path_color_0 = E_0
    // bsdf_color_0 = f_0
    // path_color_1 = E_0 + f_0 * E_1
    // bsdf_color_1 = f_0 * f_1
    // path_color_2 = E_0 + f_0 * E_1 + f_0*f_1*E_2
    //
    // path_color_(i+1) += bsdf_color_i * E_(i+1)
    // bsdf_color_(i+1) *= f_(i+1)

    vec4            path_color;
    vec4            bsdf_color;
    sampled_color_t indirect_color;
    ray_t           indirect_ray;

    scattering_t scattering = {
        .direct_color = {0, 0, 0, 0},
        .indirect_color = &indirect_color,
        .indirect_count = 1,
        .indirect_rays = &indirect_ray,
    };

    glm_vec4_zero(path_color);
    glm_vec4_one(bsdf_color);

    for (int depth = 0; depth < integrator->max_depth; depth++)
    {
        scatter(integrator->evaluator, ray, &scattering);

        // path_color_(i+1) += bsdf_color_i * E_(i+1)
        glm_vec4_muladd(bsdf_color, scattering.direct_color, path_color);

        if(path_color[1]>0.6)
        {
       //     printf("Warn: lol2\n");
        }

        if(scattering.indirect_count==0)
            break;

        // bsdf_color_(i+1) *= f_(i+1) * w
        glm_vec4_mul(bsdf_color, indirect_color.color, bsdf_color);
        float w = cos_weight(ray.direction, indirect_ray.direction);
        glm_vec4_scale(bsdf_color, w, bsdf_color);
    }

    glm_vec4_copy(path_color, result);
}


void integrate2(integrator_t* integrator, ray_t ray, vec4 result)
{
    // update scheme
    // L_i = E_i + f_i L_{i+1}
    // L_0 = E_0 + f_0 (E_1 + f_1 (E_2 + f_2 ))
    // path_color_0 = E_0
    // bsdf_color_0 = f_0
    // path_color_1 = E_0 + f_0 * E_1
    // bsdf_color_1 = f_0 * f_1
    // path_color_2 = E_0 + f_0 * E_1 + f_0*f_1*E_2
    //
    // path_color_(i+1) += bsdf_color_i * E_(i+1)
    // bsdf_color_(i+1) *= f_(i+1)

    vec4            path_color;
    vec4            bsdf_color;
    sampled_color_t indirect_color;
    ray_t           indirect_ray;

    scattering_t scattering = {
        .direct_color = {0, 0, 0, 0},
        .indirect_color = &indirect_color,
        .indirect_count = 1,
        .indirect_rays = &indirect_ray,
    };

    glm_vec4_zero(path_color);
    glm_vec4_one(bsdf_color);

    scatter(integrator->evaluator, ray, &scattering);

    if(scattering.indirect_count==0)
    {
        vec4 color = {0,0,1,1};
        glm_vec4_copy(color,result);
    }
    else{

        vec4 color = {0,1,0,1};
        glm_vec4_copy(color,result);
    }

}
