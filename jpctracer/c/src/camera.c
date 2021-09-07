
#include "camera.h"
#include "cglm/vec3.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ray_s_t generate_camera_ray(const camera_t* camera, int width, int height, vec2 pixel, vec2 rand_p)
{
    float near_plane_width = width / fmax(width, height);
    float near_plane_height = height / fmax(width, height);
    float dir_x = (pixel[0] + rand_p[0]) / width * near_plane_width * 2 - near_plane_width;
    float dir_y = (pixel[1] + rand_p[1]) / height * near_plane_height * 2 - near_plane_height;

    ray_s_t result = {
        .origin = {0, 0, 0},
        .direction = {dir_x, -dir_y, -camera->near_plane},
    };
    glm_vec3_normalize(result.direction);
    return result;
}
