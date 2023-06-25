#include "../bsdf.h"
#include "../shaders.h"
#include "../utils.h"
#include "log/log.h"


static bsdfnode_t shader(bsdfcontext_t* ctx, void* params)
{
    return mirror(ctx);
}

static shader_t shader_mirror= {
    .name = "Mirror",
    .uniforms_layout = NULL,
    .uniforms_count = 0,
    .uniforms_size = 0,
    .uniforms_default = NULL,
    .create_bsdf = shader,
};

shader_t init_shader_mirror()
{
    return shader_mirror;
}
