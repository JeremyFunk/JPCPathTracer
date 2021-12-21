#pragma once
#include "jpc_api.h"
#include "types.h"

void render_tile(const scene_t*           scene,
                 const render_settings_t* settings,
                 bounds2d_t*              tile,
                 image_t*                 output);
