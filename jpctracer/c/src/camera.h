#pragma once
#include "jpc_api.h"
#include "types.h"
#include <cglm/cglm.h>

ray_t generate_camera_ray(const camera_t* camera, int width, int height, vec2 pixel, vec2 rand_p);