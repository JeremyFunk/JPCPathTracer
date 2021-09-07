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

void render(const scene_t* scene, const render_settings_t settings,
            image_t* outputs)
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

void eval_background_color(const materiallib_t* materials, vec3 direction,
                           vec4* out_color)
{
    *out_color[0] = 0.1;
    *out_color[1] = 0.1;
    *out_color[2] = 0.1;
    *out_color[3] = 1;
}

void add_to_emission(sampled_color_t* light, sampled_color_t* bsdf,
                     bidir_scattering_t* scattering, uint64_t mask,
                     vec4* emission)
{
    vec4 temp;
    for (int i = 0; i < scattering->scattered_n; i++)
    {
        if (mask & (1 << i))
        {
            glm_vec4_scale(bsdf[i].color, 1. / bsdf[i].pdf, temp);
            glm_vec4_mul(temp, light[i].color, temp);
            float cos_weight = glm_vec3_dot(scattering->scattered_dirs[i],
                                            scattering->incident_dir);
            glm_vec4_scale(temp, cos_weight, temp);
            glm_vec4_add(temp, *emission, *emission);
        }
    }
}

typedef struct
{
    vec4 bsdf_color;
    vec4 path_color;
    int  max_depth;
    int  depth;
} render_integral_t;

render_integral_t init_integral(int max_depth)
{
    return (render_integral_t){
        .bsdf_color = {0, 0, 0, 0},
        .path_color = {1, 1, 1, 1},
        .max_depth = max_depth,
        .depth = 0,
    };
}

// update scheme
// L_i = E_i + f_i L_{i+1}
// L_0 = E_0 + f_0 (E_1 + f_1 (E_2 + f_2 ))
// path_color_0 = E_0
// bsdf_color_0 = f_0
// path_color_1 = E_0 + f_0 * E_1
// bsdf_color_1 = f_0 * f_1
// path_color_2 = E_0 + f_0 * E_1 + f_0*f_1*E_2
void integral_update(render_integral_t* integral,
                     sampled_color_t* indirect_color, vec4 emission,
                     bidir_scattering_t* scattering, int indirect_i)
{
    glm_vec4_muladd(emission, integral->bsdf_color, integral->path_color);

    float cos_weight = glm_vec4_dot(scattering->incident_dir,
                                    scattering->scattered_dirs[indirect_i]);
    vec4  f_0;
    glm_vec4_scale(indirect_color->color, cos_weight / indirect_color->pdf,
                   f_0);
    glm_vec4_mul(f_0, integral->bsdf_color, integral->bsdf_color);
}

void integral_update_background(render_integral_t* integral, vec4 background)
{

    glm_vec4_muladd(background, integral->bsdf_color, integral->path_color);
}

int integral_next(render_integral_t* integral)
{
    integral->depth++;
    return integral->depth < integral->max_depth
           && glm_vec4_norm(integral->bsdf_color) < 1e-4;
}

void render_tile(const scene_t* scene, const render_settings_t* settings,
                 tile_t* tile, image_t* output)
{
    assert(output->channels >= 4);

    int subpixels_n = settings->subpixels;
    int light_samples_x = sqrt(settings->light_samples);
    int light_samples_y = light_samples_x;
    int light_samples_n = light_samples_x * light_samples_y;

    float          camera_sample[2];
    sampler_state* sampler = sampler_init();

    bidir_scattering_t scattering;
    scattering.scattered_dirs = malloc((light_samples_n + 1) * sizeof(vec3));
    vec2* light_samples = malloc(light_samples_n * sizeof(vec2));
    vec2* subpixel_samples = malloc(subpixels_n * subpixels_n * sizeof(vec2));
    sampled_color_t* light_colors
        = malloc(light_samples_n * sizeof(sampled_color_t));
    sampled_color_t* bsdf_colors
        = malloc((light_samples_n + 1) * sizeof(sampled_color_t));

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

                render_integral_t integral = init_integral(settings->max_depth);
                vec2              pixel = {x, y};
                sample2d(sampler, 1, 1, &camera_sample);
                // clang-format off
                ray_s_t ray_incident = generate_camera_ray(&scene->camera,
                                                         output->width, 
                                                         output->height, 
                                                         pixel, 
                                                         camera_sample);

                glm_vec3_copy(ray_incident.direction, scattering.incident_dir);
                glm_vec3_copy(ray_incident.origin,scattering.hit_point.location);

                while(integral_next(&integral)) 
                {
                    if (ray_intersect(&scene->geometries, scattering.incident_dir, &scattering.hit_point))
                    {
                        glm_vec3_negate(scattering.incident_dir);

                        vec4 emission = {0, 0, 0, 0};

                        vec2 indirect_randp;

                        sample2d(sampler, light_samples_x, light_samples_y, light_samples);
                        sample2d(sampler, 1, 1, &indirect_randp);

                        sample_lights(&scene->lights, 
                                      light_samples, 
                                      light_samples_n,
                                      &scattering, //updates scattering
                                      light_colors);

                        int indirect_i = scattering.scattered_n;
                        scattering.scattered_n++;

                        bsdf_evaluate(&scene->materiallib,
                                      &scattering, //updates scattering
                                      indirect_i,
                                      indirect_randp,
                                      bsdf_colors,
                                      &emission);

                        // clang-format on
                        uint64_t shadow_mask
                            = rays_shadow_test(&scene->geometries, &scattering);

                        for (int i = 0; i < scattering.scattered_n; i++)
                            glm_vec3_normalize(scattering.scattered_dirs[i]);

                        add_to_emission(light_colors, bsdf_colors, &scattering,
                                        shadow_mask, &emission);

                        integral_update(&integral, &bsdf_colors[indirect_i],
                                        emission, &scattering, indirect_i);

                        glm_vec3_copy(scattering.scattered_dirs[indirect_i],
                                      scattering.incident_dir);
                    }
                    else
                    {
                        vec4 background;
                        eval_background_color(&scene->materiallib,
                                              scattering.incident_dir,
                                              &background);
                        integral_update_background(&integral, background);
                    }
                }
                glm_vec4_add(integral.path_color, pixel_color, pixel_color);
            }
            glm_vec4_scale(pixel_color, 1. / subpixels_n, pixel_color);
            int pixel_idx = output->width * output->channels * y_i
                            + x_i * output->channels;
            for (int i = 0; i < 4; i++)
                output->data[pixel_idx + i] = pixel_color[i];
        }
    }

    free(scattering.scattered_dirs);
    free(light_samples);
    free(subpixel_samples);
    free(light_colors);
    free(bsdf_colors);
    sampler_free(sampler);
}
