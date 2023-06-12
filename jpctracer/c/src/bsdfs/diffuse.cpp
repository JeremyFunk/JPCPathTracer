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

struct diffuse_params
{
    vec4 color;

    void eval(vec3             incident_dir,
              vec3*            scattered_dirs,
              uint             n,
              sampled_color_t* out_colors,
              vec4*            emission)
    {
        vec4 luminance;
        glm_vec4_scale(color, 1./M_PI, luminance);
        luminance[3] = 1;
        for (uint i = 0; i < n; i++)
        {
            if (same_hemisphere_nspace(incident_dir, scattered_dirs[i]))
            {

                out_colors[i].pdf = std::fabs(cos_theta(scattered_dirs[i])) / M_PI;

                glm_vec4_ucopy(luminance, out_colors[i].color);

            }
            else
            {
                out_colors[i].pdf = 0;
                glm_vec4_zero(out_colors[i].color);
            }
                

        }
        vec4 tmp = {0, 0, 0, 1};
        glm_vec4_ucopy(tmp, *emission);
    }
    void sample(vec3 incident_dir, vec2 rand_p, vec3* out_scattered_dir)
    {
        cosinus_sample_hemisphere(rand_p, *out_scattered_dir);
        if (!same_hemisphere_nspace(*out_scattered_dir, incident_dir))
        {
            glm_vec3_negate(*out_scattered_dir);
        }
    }

    diffuse_params(float* _color)
    {
        for (int i = 0; i < 4; i++)
            color[i] = _color[i];
    }
};

bsdfnode_t diffuse(bsdfcontext_t* ctx,float4 color)
{
    return bsdfshader_create<diffuse_params>(ctx,color);
}