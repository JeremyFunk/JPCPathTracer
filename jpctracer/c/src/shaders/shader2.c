/*
#include "../bsdf.h"

bsdf* diffuse(bsdfcontext* ctx, const vec4* color);

typedef struct
{
    vec4 alberto;
    vec4 normal;
    float roughness;
    float metalic;
} shader_params;

bsdf* shader1(bsdfcontext* ctx, void* params)
{
    shader_params* p = params;
    return diffuse(ctx, &p->alberto);
}

shader* create_shader1()
{
    shader* s = shader_init("shader1", shader1, sizeof(shader_params));
    shader_params* p = shader_default_params(s);
    vec4 black = {0};
    p->alberto[0] = black[0];
    p->roughness = 0;
    p->metalic = 0;
    p->normal[0] = black[0];

    shader_bind_color(s, &p->alberto, "alberto");
    shader_bind_color(s, &p->normal, "normal");
    shader_bind_value(s, &p->roughness, "roughness");
    shader_bind_value(s, &p->metalic, "metalic");
    return s;

    REGISTER_SHADER(shader1, shader_params);
    REGISTER_SHADER_PARAM(roughness)
    REGISTER_SHADER_PARAM(normal)
    ...
}*/
