#pragma once
#include "bsdf.h"
#include <jpc_api.h>

// returns the used samples
uint sample_lights(const lights_t*  lights,
                   vec2*            rand_points,
                   int              n,
                   hit_point_t      hit_p,
                   vec3*            out_directions,
                   float*           out_distances,
                   sampled_color_t* out_colors);
