#pragma once
#include "allocators.h"
#include "jpc_api.h"
#include <cglm/cglm.h>
#include <stdint.h>


void* material_load_params(const material_t* material, const image_t* textures, vec2 uv, stack_allocator_t* allocator);


shader_t init_shader_diffuse();
// clang-format on
