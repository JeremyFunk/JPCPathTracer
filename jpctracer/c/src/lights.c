#include "lights.h"
#include "bsdf.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "sampling.h"

int sample_point_lights(point_light_t*   lights,
                        uint             lights_n,
                        int              samples_n,
                        hit_point_t      hitpoint,
                        vec2*            rand_points,
                        vec3*            out_directions,
                        float*           out_distances,
                        sampled_color_t* out_colors)
{
    for (int i = 0; i < samples_n; i++)
    {
        int           light_i = rand_points[i][0] * samples_n;
        point_light_t light = lights[light_i];

        glm_vec3_sub(light.position, hitpoint.location, out_directions[i]);
        float dist2 = glm_vec3_norm2(out_directions[i]);
        float dist = sqrt(dist2);
        glm_vec3_scale(out_directions[i], 1. / dist, out_directions[i]);

        glm_vec4_scale(
            lights[i].color, lights[i].strength / dist2, out_colors[i].color);
        out_colors[i].pdf = 1. / samples_n;
        out_distances[i] = dist;
        out_colors[i].color[3] = 1;
    }
    return samples_n;
}

int sample_lights(const lights_t*  lights,
                  vec2*            rand_points,
                  int              n,
                  hit_point_t      hitpoint,
                  vec3*            out_directions,
                  float*           out_distances,
                  sampled_color_t* out_colors)
{
    int   all_sample_count = 0;
    uint  all_lights = lights->point_lights_count + lights->sun_lights_count;
    float point_light_pdf
        = (float)lights->point_lights_count / (float)all_lights;
    float sun_light_pdf = (float)lights->sun_lights_count / (float)all_lights;

    int point_light_sample_count = point_light_pdf * n;

    sampled_color_t* temp_colors = out_colors;

    point_light_sample_count = sample_point_lights(lights->point_lights,
                                                   lights->point_lights_count,
                                                   point_light_sample_count,
                                                   hitpoint,
                                                   rand_points,
                                                   out_directions,
                                                   out_distances,
                                                   temp_colors);

    for (int i = 0; i < point_light_sample_count; i++)
        temp_colors[i].pdf *= point_light_pdf;

    out_directions += point_light_sample_count;
    temp_colors += point_light_sample_count;
    all_sample_count += point_light_sample_count;
    return all_sample_count;
}
