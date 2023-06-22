#include "integrator.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"
#include "sampler.h"
#include "scattering.h"
#include "types.h"
#include "log/log.h"
#include "config.h"

#include <stdlib.h>

typedef struct integrator_s
{
    ray_evaluator_t* evaluator;
    uint             max_depth;
    int passes;
} integrator_t;

integrator_t* integrator_init(int            max_depth,
                              const scene_t* scene,
                              sampler_state* sampler,
                              uint           light_samples,
                              int passes)
{
    integrator_t* result = malloc(sizeof(*result));
    result->evaluator = ray_evaluator_init(scene, sampler, light_samples, 1);
    result->max_depth = max_depth;
    result->passes = passes;
    return result;
}

void integrator_free(integrator_t* integrator)
{
    ray_evaluator_free(integrator->evaluator);
}


void integrate(integrator_t* integrator, ray_t ray, float* result)
{
    // update scheme
    // L_i = E_i + f_i L_{i+1}
    // L_0 = E_0 + f_0 (E_1 + f_1 (E_2 + f_2 ))
    // path_color_0 = E_0
    // bsdf_color_0 = f_0
    // path_color_1 = E_0 + f_0 * E_1
    // bsdf_color_1 = f_0 * f_1
    // path_color_2 = E_0 + f_0 * E_1 + f_0 * f_1 * E_2
    //
    // path_color_(i+1) = path_color_(i) + bsdf_color_i * E_(i+1)
    // bsdf_color_(i+1) = bsdf_color_(i) * f_(i+1)

    vec4            path_color;
    vec4            bsdf_color;
    vec4            indirect_color;
    ray_t           indirect_ray;

    scattering_t scattering = {
        .direct_color = {0, 0, 0, 0},
        .indirect_colors = &indirect_color,
        .indirect_count = 0,
        .indirect_rays = &indirect_ray,
        .normal = {0,0,0},
    };

    glm_vec4_zero(path_color);
    glm_vec4_one(bsdf_color);

    
    uint depth = 0;

    for (; depth < integrator->max_depth; depth++)
    {
#ifdef INTEGRATOR_LOG
        printf("ray origin: (%f,%f,%f) direction: (%f,%f,%f)\n",
               ray.origin[0],
               ray.origin[1],
               ray.origin[2],
               ray.direction[0],
               ray.direction[1],
               ray.direction[2]);
#endif
        scatter(integrator->evaluator, ray, &scattering);

        #ifdef INTEGRATOR_LOG
        vec4 old_path_color;
        vec4 old_bsdf_color;
        glm_vec4_copy(path_color,old_path_color);
        glm_vec4_copy(bsdf_color,old_bsdf_color);

#endif // INTEGRATOR_LOG

        // path_color_(i+1) = path_color(i) + bsdf_color_i * E_(i+1)
        glm_vec4_muladd(bsdf_color, scattering.direct_color, path_color);

#ifdef INTEGRATOR_LOG 
        printf("path_color_(%d) = path_color(%d) + bsdf_color_(%d) * E_(%d)\n",
                 depth + 1,
                 depth,
                 depth,
                 depth + 1);
        for(uint i = 0; i<4;i++) 
        {
            printf("%f = %f + %f * %f\n ",path_color[i],old_path_color[i],bsdf_color[i],scattering.direct_color[i]);
        }
#endif


        if (integrator->passes & JPC_PASS_NORMAL && depth == 0)
        {
            glm_vec3_copy(scattering.normal, result + 4);
        }

        if (scattering.indirect_count == 0)
        {
#ifdef INTEGRATOR_LOG
       printf("no hit\n");
#endif
            break;
        }

        if( glm_vec3_eq_eps( scattering.indirect_colors,0) )
        {
            #ifdef INTEGRATOR_LOG
            printf("Indirect color is zero\n");
            #endif
            break;
        }

        // bsdf_color_(i+1) *= f_(i+1) 
        glm_vec4_mul(bsdf_color, indirect_color, bsdf_color);
        ray = indirect_ray;

        #ifdef INTEGRATOR_LOG
            printf("bsdf_color(%d) = bsdf_color(%d) * f_(%d)\n",depth+1,depth,depth+1);
            for(uint i =0; i<4;i++)
            printf("%f = %f * %f\n",
                   bsdf_color[i],
                   old_bsdf_color[i],
                   indirect_color[i]);
        #endif

        
    }

    //save output

    uint passes_count = 4;
    glm_vec4_copy(path_color, result);
    if(integrator->passes & JPC_PASS_NORMAL)
    {
        //already written
        passes_count += 3;
    }
    if(integrator->passes & JPC_PASS_DEPTH)
    {
        result[passes_count] = depth;
        passes_count++;
    }


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
        .indirect_colors = &indirect_color,
        .indirect_count = 1,
        .indirect_rays = &indirect_ray,
    };

    glm_vec4_zero(path_color);
    glm_vec4_one(bsdf_color);

    scatter(integrator->evaluator, ray, &scattering);

    if (scattering.indirect_count == 0)
    {
        vec4 color = {0, 0, 1, 1};
        glm_vec4_copy(color, result);
    }
    else
    {

        vec4 color = {0, 1, 0, 1};
        glm_vec4_copy(color, result);
    }
}
