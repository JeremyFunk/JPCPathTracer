#include "../bsdf.h"
#include "../sampling.h"
#include "../spherical_coords.h"
#include "cglm/struct/vec3.h"
#include "cglm/struct/vec4.h"
#include "jpc_api.h"

typedef struct
{
    float4 color;
} diffuse_params;

void diff_eval(const bidir_scattering_t* rays, sampled_color_t* out_colors, vec4* emission, void* params)
{
    diffuse_params* p = params;
    float4 luminance;
    glm_vec4_scale(p->color, M_PI, luminance);
    for (int i = 0; i < rays->scattered_n; i++)
    {
        float pdf = same_hemisphere_nspace(rays->incident_dir, rays->scattered_dirs[i])
                        ? pdf = fabs(cos_theta(rays->scattered_dirs[i])) / M_PI
                        : 0;
        glm_vec4_copy(luminance, out_colors[i].color);

        out_colors[i].pdf = pdf;
    }
}

void diff_sample(vec3 incident_dir, vec2 rand_p, vec3* out_scattered_dir, void* params)
{
    cosinus_sample_hemisphere(rand_p, out_scattered_dir);
    if (!same_hemisphere_nspace(*out_scattered_dir, incident_dir))
        glm_vec3_negate(*out_scattered_dir);
}

bsdfnode_t diffuse(bsdfcontext_t* ctx, float4 color)
{

    diffuse_params* param = stack_alloc(&ctx->params_allocator, sizeof(diffuse_params), _Alignof(diffuse_params));
    glm_vec4_copy(color, param->color);
    return bsdfshaders_add(&ctx->shaders, diff_eval, diff_sample, param);
}