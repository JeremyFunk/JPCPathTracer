extern "C"
{
#include "../bsdf.h"
#include "../sampling.h"
#include "../spherical_coords.h"
#include "cglm/struct/vec3.h"
#include "cglm/struct/vec4.h"
#include "jpc_api.h"
}
#include "../bsdf.hpp"
#include<cmath>

void calc_reflected_dir(vec3 dir, vec3 dest)
{
    dest[0] = -dir[0];
    dest[1] = -dir[1];
    dest[2] = dir[2];
}

struct mirror_params
{
    
    void eval(vec3             incident_dir,
              vec3*            scattered_dirs,
              uint             n,
              sampled_color_t* out_colors,
              vec4*            emission)
    {
        vec4 zeros = {0, 0, 0, 1};
        vec4 ones = {1, 1, 1, 1};

        vec3 reflected_dir;
        calc_reflected_dir(incident_dir, reflected_dir);

        for (uint i = 0; i < n; i++)
        {
            if (glm_vec3_eqv_eps(scattered_dirs[i], reflected_dir))
            {
                glm_vec4_copy(ones, out_colors[i].color);
                out_colors[i].pdf = 1;
            }
            else
            {
                glm_vec4_copy(zeros, out_colors[i].color);
                out_colors[i].pdf = 0;
            }
        }
        glm_vec4_copy(zeros, *emission);
    }
    void sample(vec3 incident_dir, vec2 rand_p, vec3* out_scattered_dir)
    {
        calc_reflected_dir(incident_dir, *out_scattered_dir);
        return;
        
    }

    mirror_params()
    {

    }
};

bsdfnode_t mirror(bsdfcontext_t* ctx)
{
    return bsdfshader_create<mirror_params>(ctx);
}

