#include "shaders.h"
#include "allocators.h"
#include "cglm/vec3.h"
#include "texture.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void* material_load_params(const material_t* material, const image_t* textures, vec2 uv, stack_allocator_t* allocator)
{

    void* params = stack_alloc(allocator, material->param_size, _Alignof(float4));

    uintptr_t params_int = (uintptr_t)params;

    memcpy(params, material->params, material->param_size);

    for (int i = 0; i < material->bindings_count; i++)
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

shaders_t* shaders_init()
{
    shaders_t* shader = malloc(sizeof(shaders_t));

    shader->shaders = malloc(SHADER_COUNT * sizeof(shader_t*));
    shader->count = SHADER_COUNT;

    return shader;
}

void shader_load_defaults(shaders_t* shaders)
{
    shaders->shaders[0] = init_shader_diffuse();
}
void shaders_free(shaders_t* shaders)
{
    free(shaders->shaders);
    free(shaders);

}
