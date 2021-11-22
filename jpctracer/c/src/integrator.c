#include "integrator.h"

render_integral_t integral_init(int max_depth)
{
    return (render_integral_t){
        .indirect_color = {0, 0, 0, 0},
        .direct_color = {1, 1, 1, 1},
        .max_depth = max_depth,
        .depth = 0,
    };
}

bool integral_next(render_integral_t* integral, const scattering_t* scattering, ray_t* next_ray)
{
    return false;
}

void integral_get_color(render_integral_t* integral, vec4* out_color)
{
    glm_vec4_copy(integral->direct_color,*out_color);
}
