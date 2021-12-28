#include "renderer.h"
#include "allocators.h"
#include "bsdf.h"
#include "bvh.h"
#include "camera.h"
#include "cglm/struct/vec3.h"
#include "cglm/struct/vec4.h"
#include "cglm/types.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"
#include "lights.h"
#include "log/log.h"
#include "sampler.h"
#include "types.h"
#include <assert.h>
#include <cglm/struct.h>
#include <jpc_api.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "integrator.h"
#include "utils.h"
#include <math.h>

void render(const scene_t*          scene,
            const render_settings_t settings,
            image_t*                outputs)
{
    log_info("Start rendering");
    log_debug("test\n");

    for(int i=0;i<outputs->channels*outputs->width*outputs->height;i++)
    {
        outputs->data[i] = 0;
    }

    uint width = outputs->width;
    uint height = outputs->height;
    uint tile_s = settings.tile_size;

    uint tiles_count_x = ceil((float)width / (float)settings.tile_size);
    uint tiles_count_y = ceil((float)height / (float)settings.tile_size);

    for (int y = 0; y < tiles_count_y; y++)
    {
        for (int x = 0; x < tiles_count_x; x++)
        {
            bounds2d_t tile
                = {.min = {x * tile_s, y * tile_s},
                   .max = {
                       MIN((x + 1) * tile_s, width),
                       MIN((y + 1) * tile_s, height),},
                };
            log_info("next tile");
            render_tile(scene, &settings, &tile, outputs);
        }
    }
    log_info("End rendering");
}
/*
void render_tile2(const scene_t*           scene,
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

                    scattering_init(&scattering, 1, ray.direction);
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

                        bsdf_init(bsdf,
                                  &scene->materiallib,
                                  scattering.incident_dir,
                                  scattering.hitpoint);

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
                        eval_background(&scene->materiallib,
                                        ray.direction,
                                        scattering.emission);
                    }
                } while (integral_next(&integral, &scattering, &ray));

                vec4 integral_color;
                integral_get_color(&integral, &integral_color);
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
}*/

void add_to_image(image_t* image, vec2 pixel, float* color, uint subpixels)
{
    uint2  idx = {pixel[0], pixel[1]};
    uint c = image->channels;
    float* img_pixel = image->data + idx[1] * image->width * c + idx[0] * c;

    for (int i = 0; i < image->channels; i++)
        img_pixel[i] += color[i] / subpixels;
}

void render_tile(const scene_t*           scene,
                 const render_settings_t* settings,
                 bounds2d_t*              tile,
                 image_t*                 output)
{
    sampler_state* sampler = sampler_init();
    integrator_t*  integrator = integrator_init(
        settings->max_depth, scene, sampler, settings->light_samples);
    iterator2d pixel_iter = grid2d(*tile, settings->subpixels);

    vec2 pixel;
    vec4 color;
    while (sample2d_next(&pixel_iter, sampler, pixel))
    {
        ray_t ray = generate_camera_ray(
            &scene->camera, output->width, output->height, pixel);
        integrate(integrator, ray, color);
        add_to_image(output, pixel, color, settings->subpixels);
    }

    integrator_free(integrator);
    sampler_free(sampler);
}
