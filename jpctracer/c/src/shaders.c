#include "shaders.h"
#include "allocators.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "log/log.h"
#include "texture.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* material_load_params(const material_t*  material,
                           const image_t*     textures,
                           vec2               uv,
                           stack_allocator_t* allocator)
{

    void* params
        = stack_alloc(allocator, material->param_size, _Alignof(float4));

    uintptr_t params_int = (uintptr_t)params;

    memcpy(params, material->params, material->param_size);

    for (uint i = 0; i < material->bindings_count; i++)
    {

        texture_uniform_binding_t binding = material->bindings[i];
        image_t                   texture = textures[binding.texture];
        uintptr_t                 dst_int = params_int + binding.offset;

        float* param_dst = (float*)dst_int;
        switch (binding.type)
        {
        case JPC_float:
            texture_get_float(texture, uv, param_dst);
            break;
        case JPC_float3:
            texture_get_float3(texture, uv, param_dst);
            break;
        case JPC_float4:
            texture_get_float4(texture, uv, param_dst);
            break;
        }
    }

    return params;
}

#define SHADER_COUNT 1

shaders_t shaders_init()
{
    shaders_t shader;

    shader.shaders = malloc(SHADER_COUNT * sizeof(shader_t));
    shader.count = SHADER_COUNT;

    return shader;
}

void shaders_load_defaults(shaders_t shaders)
{
    shaders.shaders[0] = init_shader_diffuse();
}
void shaders_free(shaders_t shaders)
{
    free(shaders.shaders);
}

size_t sizeof_uniform(uniform_type_t type)
{
    switch (type)
    {
    case JPC_float:
        return sizeof(float);
    case JPC_float3:
        return sizeof(float3);
    case JPC_float4:
        return sizeof(float4);
    }
    assert(false);
    return 0;
}

void shader_default_uniform(const shader_t* shader, uint id, float* dst)
{

    char* uniform = (char*)shader->uniforms_default;
    for (uint i = 0; i < id; i++)
    {
        uniform += sizeof_uniform(shader->uniforms_layout[i].type);
    }
    memccpy(dst,
            uniform,
            sizeof(char),
            sizeof_uniform(shader->uniforms_layout[id].type));
}

typedef struct mat_bfr_s
{
    char*                      params;
    texture_uniform_binding_t* bindings;
} mat_bfr_t;

void mat_bfr_t_free(mat_bfr_t* bfr)
{
    free(bfr->params);
    free(bfr->bindings);
    free(bfr);
}

mat_bfr_t* materials_init(material_t*     materials,
                          const shader_t* shaders,
                          uint            n)
{
    size_t params_size = 0;
    size_t bindings_count = 0;
    for (uint i = 0; i < n; i++)
    {
        params_size += shaders[i].uniforms_size;
        bindings_count += shaders[i].uniforms_count;
    }

    mat_bfr_t* bfr = malloc(sizeof(mat_bfr_t));

    if (n == 0)
    {
        bfr->bindings = NULL;
        bfr->params = NULL;
        return bfr;
    }

    bfr->params = malloc(sizeof(char) * params_size);
    bfr->bindings = malloc(sizeof(texture_uniform_binding_t) * bindings_count);

    texture_uniform_binding_t* tex_uni_buffer = bfr->bindings;
    char*                      params_buffer = bfr->params;

    for (uint i = 0; i < n; i++)
    {

        materials[i].bindings_count = 0;
        materials[i].bindings = tex_uni_buffer;
        tex_uni_buffer += shaders[i].uniforms_count;

        materials[i].params = params_buffer;
        params_buffer += shaders[i].uniforms_size;
        memcpy(materials[i].params,
               shaders[i].uniforms_default,
               shaders[i].uniforms_size);

        materials[i].param_size = shaders[i].uniforms_size;
        materials[i].bsdf_creator = shaders[i].create_bsdf;
    }

    return bfr;
}

void material_set_uniform(material_t*     mat,
                          const shader_t* shader,
                          uint            uniform_id,
                          float*          value)
{
    char* uniform = mat->params;
    for (uint i = 0; i < uniform_id; i++)
    {
        uniform += sizeof_uniform(shader->uniforms_layout[i].type);
    }
    memcpy(uniform,
           value,
           sizeof_uniform(shader->uniforms_layout[uniform_id].type));
}
void material_set_texture(material_t*     mat,
                          const shader_t* shader,
                          uint            uniform_id,
                          uint            texture)
{
    uintptr_t offset = 0;

    for (uint i = 0; i < uniform_id; i++)
    {
        offset += sizeof_uniform(shader->uniforms_layout[i].type);
    }

    uint binding_i = 0;
    while (binding_i < mat->bindings_count
           && mat->bindings[binding_i].offset == offset)
    {
        binding_i++;
    }

    mat->bindings[binding_i].offset = offset;
    mat->bindings[binding_i].type = shader->uniforms_layout[uniform_id].type;
    mat->bindings[binding_i].texture = texture;
}
