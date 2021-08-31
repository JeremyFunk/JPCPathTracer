#include "integrator.h"
#include <jpc_api.h>
#include <math.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void render(const scene_t* scene, const render_settings_t settings, image_t* outputs)
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
