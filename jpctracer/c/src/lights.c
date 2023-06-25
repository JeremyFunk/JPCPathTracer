#include "lights.h"
#include "bsdf.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "sampling.h"
#include "utils.h"

uint sample_point_lights(point_light_t*   lights,
                         uint             lights_n,
                         sampled_color_t* out_colors,
                         vec3 location,
                         vec3* out_directions,
                         float* out_distances
)
{
    assert(lights_n > 0);
    for (uint i = 0; i < lights_n; i++)
    {
        point_light_t light = lights[i];

        glm_vec3_sub(light.position,location, out_directions[i]);
        float dist2 = glm_vec3_norm2(out_directions[i]);
        float dist = sqrt(dist2);
        glm_vec3_scale(out_directions[i], 1. / dist, out_directions[i]);
        vec4 out_tmp_color;
        vec4 light_color;
        glm_vec4_ucopy(lights[i].color, light_color);
        
        glm_vec4_scale(
            light_color, lights[i].strength / dist2, out_tmp_color);
        glm_vec3_copy(out_tmp_color, out_colors[i].color);
        out_colors[i].pdf = 0;
        out_distances[i] = dist;
        out_colors[i].color[3] = 1;
    }
    return lights_n;
}

void sample_lights(const lights_t*  lights, vec2* rand_p,
 uint rand_count, vec3 hit_location, scattering_rays_t* scattering_rays)
{
    uint  all_sample_count = 0;
    uint  all_lights = lights->point_lights_count + lights->sun_lights_count;
    assert(all_lights == scattering_rays->light_delta_count);
    sample_point_lights(lights->point_lights,
                        lights->point_lights_count,
                        scattering_rays->light_colors,
                        hit_location,
                        scattering_rays->ray_directions+scattering_rays->indirect_rays_count,
                        scattering_rays->distances);

    scattering_rays->light_system = JPC_COORD_WORLD; 
                                                

    return all_sample_count;
}

void scattering_rays_reset(const lights_t* lights,uint indirect_rays,
 uint light_samples, scattering_rays_t* rays)
{
    rays->indirect_rays_count = indirect_rays;
    rays->light_delta_count = lights->point_lights_count;    
    rays->lights_sampled_count = 0;
    rays->light_system = JPC_COORD_NOTSET;
    rays->indirect_system = JPC_COORD_NOTSET;
}

scattering_rays_t scattering_rays_alloc(arena_t* arena,const lights_t* lights, uint indirect_rays, uint light_samples)
{
    scattering_rays_t rays;

    scattering_rays_reset(lights, indirect_rays, light_samples, &rays);

    uint light_rays_count = rays.light_delta_count+rays.lights_sampled_count;
    uint all_rays_count = light_rays_count+rays.indirect_rays_count;

    size_t light_colors_size = light_rays_count*sizeof(sampled_color_t);
    size_t bsdf_colors_size = all_rays_count*sizeof(sampled_color_t);
    size_t ray_dirs_size = all_rays_count*sizeof(vec3) ;
    size_t distances_size = light_rays_count*sizeof(float);

    arena_reserve(arena,light_colors_size+bsdf_colors_size+_Alignof(sampled_color_t)
    +ray_dirs_size+_Alignof(vec3)+distances_size+_Alignof(float)
    );

    rays.light_colors =  arena_alloc_aligned(arena,light_colors_size,_Alignof(sampled_color_t));
    rays.bsdf_colors = arena_alloc_aligned(arena,bsdf_colors_size,_Alignof(sampled_color_t));
    rays.ray_directions = arena_alloc_aligned(arena,ray_dirs_size,_Alignof(vec3));
    rays.distances = arena_alloc_aligned(arena,distances_size,_Alignof(float));

    return rays;
}
