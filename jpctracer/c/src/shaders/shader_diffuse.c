#include "../bsdf.h"
#include "../shaders.h"
#include "../utils.h"
#include "log/log.h"

// reflection fehlt
static uniform_desc_t layout[] = {
    {JPC_float, "roughness"},
    {JPC_float4, "color"},
};
// should have the same arrangment as layout
typedef struct params_s
{
    float roughness;
    float color[4];
} params_t;

static params_t defaults = {
    .roughness = 0.4,
    .color = {0, 2, 1, 3},
};

static bsdfnode_t shader(bsdfcontext_t* ctx, void* _params)
{
    params_t* p = _params;
    p->color[0] = ctx->hit.instance_id;
    if (ctx->hit.instance_id == 0)
    {
//        log_info("test");
    }

    return diffuse(ctx,p->color);
}


static shader_t shader_diffuse = {
    .name = "Diffuse",
    .uniforms_layout = layout,
    .uniforms_count = COUNT_OF(layout),
    .uniforms_size = sizeof(params_t),
    .uniforms_default = &defaults,
    .create_bsdf = shader,
};

shader_t init_shader_diffuse()
{
    return shader_diffuse;
}
