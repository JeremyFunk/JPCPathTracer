#pragma once
#include "jpc_api.h"
typedef struct
{
    int x_start;
    int x_end;
    int y_start;
    int y_end;
} tile_t;
void render_tile(const scene_t* scene, const render_settings_t* settings, tile_t* tile, image_t* output);
