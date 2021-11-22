#include "renderer.h"
#include "allocators.h"
#include "bsdf.h"
#include "bvh.h"
#include "camera.h"
#include "cglm/struct/vec3.h"
#include "cglm/struct/vec4.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "lights.h"
#include "sampler.h"
#include "types.h"
#include <assert.h>
#include <cglm/struct.h>
#include <jpc_api.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"
#include <math.h>
#include "integrator.h"

scattering_t scattering_alloc(uint max_size)
{
    scattering_t result;
    result.all_org_colors = malloc(sizeof(*result.all_org_colors)*max_size);
    result.all_directions = malloc(sizeof(*result.all_directions)*max_size);
    result.direct_dest_colors = malloc(sizeof(*result.direct_dest_colors)*max_size);
    result.direct_org_colors = NULL;
    result.direct_directions = NULL;
    result.indirect_org_colors = result.all_org_colors;
    result.indirect_directions = result.all_directions;
    result.all_size=0;
    result.indirect_size = 0;
    result.direct_size = 0;
    result.max_size = max_size;
    return result;
    
}
void scattering_free(scattering_t scatter)
{
    free(scatter.all_org_colors);
    free(scatter.all_directions);
    free(scatter.direct_dest_colors);
}

void set_to_black(vec4 color)
{
    color[0]=0;
    color[1]=0;
    color[2]=0;
    color[3]= 0;
}

void scattering_init(scattering_t* scattering,uint indirect_size,vec3 incident_dir)
{
    //TO DO
    assert(false);
    assert(true);
    assert(indirect_size >=scattering->max_size);
    scattering->direct_org_colors = scattering->all_org_colors+indirect_size;
    scattering->direct_directions = scattering->all_directions+indirect_size;
    scattering->all_size = indirect_size;
    scattering->indirect_size = indirect_size;
    set_to_black(scattering->emission);
    glm_vec3_scale(incident_dir,-1,scattering->incident_dir);
}




void scattering_resize_direct(scattering_t* scattering,uint direct_size)
{
    assert(scattering->indirect_size+direct_size >= scattering->all_size);
    scattering->all_size+=direct_size;
    scattering->direct_size = direct_size;
}

void eval_background(const materiallib_t* matlib,vec3 incident_dir, vec4 out_color)
{
    out_color[0] = 0.01;
    out_color[1] = 0.2;
    out_color[2] = 0.5;
    out_color[3] = 1;
}

void render(const scene_t*          scene,
            const render_settings_t settings,
            image_t*                outputs)
{
    uint width = outputs->width;
    uint height = outputs->height;
    uint tile_s = settings.tile_size;

    uint tiles_count_x = ceil((float)width / (float)settings.subpixels);
    uint tiles_count_y = ceil((float)height / (float)settings.subpixels);

    for (int y = 0; y < tiles_count_y; y++)
    {
        for (int x = 0; x < tiles_count_x; x++)
        {
            tile_t tile = {
                .x_start = x * tile_s,
                .x_end = MIN((x + 1) * tile_s, width),
                .y_start = y * tile_s,
                .y_end = MIN((y + 1) * tile_s, height),
            };

            render_tile(scene, &settings, &tile, outputs);
        }
    }
}


void render_tile(const scene_t*           scene,
                 const render_settings_t* settings,
                 tile_t*                  tile,
                 image_t*                 output)
{
    assert(output->channels >= 4);

    int subpixels_n = settings->subpixels;
    int light_samples_x = sqrt(settings->light_samples);
    int light_samples_y = light_samples_x;
    int light_samples_n = light_samples_x * light_samples_y;

    float          camera_sample[2];
    sampler_state* sampler = sampler_init();

    vec2* light_samples = malloc(light_samples_n * sizeof(vec2));
    vec2* subpixel_samples = malloc(subpixels_n * subpixels_n * sizeof(vec2));

    bsdfcontext_t* bsdf = bsdf_alloc(bsdf_default_limits);
    scattering_t   scattering = scattering_alloc(light_samples_n + 1);

    for (int y_i = tile->y_start; y_i < tile->y_start; y_i++)
    {
        for (int x_i = tile->x_start; x_i < tile->x_end; x_i++)
        {
            vec4 pixel_color = {0, 0, 0, 0};
            sample2d(sampler, subpixels_n, subpixels_n, subpixel_samples);
            for (int subpixel_i = 0; subpixel_i < subpixels_n * subpixels_n;
                 subpixel_i++)
            {

                float x = (float)x_i + subpixel_samples[subpixel_i][0];
                float y = (float)y_i + subpixel_samples[subpixel_i][1];

                render_integral_t integral = integral_init(settings->max_depth);
                vec2              pixel = {x, y};
                sample2d(sampler, 1, 1, &camera_sample);
                ray_t ray = generate_camera_ray(&scene->camera,
                                    output->width,
                                    output->height,
                                    pixel,
                                    camera_sample);

                do
                {

                    scattering_init(&scattering, 1,ray.direction);
                    if (ray_intersect(
                            &scene->geometries, ray, &scattering.hitpoint))
                    {

                        vec2 indirect_randp;

                        sample2d(sampler,
                                 light_samples_x,
                                 light_samples_y,
                                 light_samples);
                        sample2d(sampler, 1, 1, &indirect_randp);

                        uint direct_count  
                            = sample_lights(&scene->lights,
                                            light_samples,
                                            light_samples_n,
                                            scattering.hitpoint,
                                            scattering.direct_directions,
                                            scattering.direct_dest_colors);

                        scattering_resize_direct(&scattering, direct_count);

                        // clang-format on
                        uint64_t shadow_mask
                            = rays_shadow_test(&scene->geometries,
                                               scattering.direct_directions,
                                                ray.origin, 
                                               light_samples_n);

                        sampled_color_t black = {
                            .color = {0.f, 0.f, 0.f},
                            .pdf = 0,
                        };
                        for (int i = 0; i < scattering.direct_size; i++)
                        {
                            glm_vec3_normalize(scattering.direct_directions[i]);
                            if (shadow_mask & (1 << i))
                            {
                                scattering.direct_dest_colors[i] = black;
                            }
                        }

                        bsdf_init(
                            bsdf, &scene->materiallib, scattering.incident_dir, scattering.hitpoint);

                        bsdf_vec3_to_local(bsdf,
                                           scattering.direct_directions,
                                           scattering.direct_size);
                        bsdf_sample(bsdf,
                                    indirect_randp,
                                    scattering.indirect_directions);
                        bsdf_eval(bsdf,
                                  scattering.all_directions,
                                  light_samples_n + 1,
                                  scattering.all_org_colors,
                                  &scattering.emission);

                        bsdf_vec3_to_world(bsdf,
                                           scattering.all_directions,
                                           scattering.all_size);
                    }
                    else
                    {
                        vec4 emission;
                        eval_background(
                            &scene->materiallib, ray.direction, scattering.emission);
                    }
                } while (integral_next(&integral,&scattering,&ray));
                
                vec4 integral_color;
                integral_get_color(&integral,&integral_color);
                glm_vec4_add(integral_color, pixel_color, pixel_color);
            }

            glm_vec4_scale(pixel_color, 1. / subpixels_n, pixel_color);
            int pixel_idx = output->width * output->channels * y_i
                            + x_i * output->channels;
            for (int i = 0; i < 4; i++)
                output->data[pixel_idx + i] = pixel_color[i];
        }
    }

    scattering_free(scattering);
    free(light_samples);
    free(subpixel_samples);
    sampler_free(sampler);
    bsdf_free(bsdf);
}
