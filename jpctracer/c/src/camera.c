
#include "camera.h"
#include "cglm/vec3.h"
#include "log/log.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "bvh/shapes.h" 

ray_t generate_camera_ray(const camera_t* camera,
                          int             width,
                          int             height,
                          vec2            pixel)
{
    float near_plane_width = width / fmax(width, height);
    float near_plane_height = height / fmax(width, height);
    float dir_x = pixel[0] / width * near_plane_width * 2 - near_plane_width;
    float dir_y = pixel[1] / height * near_plane_height * 2 - near_plane_height;

    
    ray_t local_ray = {
        .direction = {dir_x, -dir_y, -camera->near_plane},
        .origin = {0, 0, 0},
        .clip_end = camera->clip_end,
    };
    ray_t world_ray;
    mat4 transform;
    glm_mat4_ucopy(camera->transformation,transform);
    ray_transform(&local_ray,transform,&world_ray);
    //glm_vec3_normalize(result.direction); This does already ray_transform

    return world_ray;
}
