#pragma once
#include <utility>
extern "C"
{
#include "bsdf.h"
#include "jpc_api.h"
}
#include "allocators.hpp"
/*
Concept:
struct bsdfshader
{
    void eval(vec3             incident_dir,
              vec3*            scattered_dir,
              uint             n,
              sampled_color_t* out_color,
              vec4*            emission);

    void sample(vec3 incident_dir, vec2 rand_p, vec3* out_scattered_dir);
};

*/

template <class T, class... Args> // T models bsdfshader
bsdfnode_t bsdfshader_create(bsdfcontext_t* ctx, Args&&... args)
{
    T* params
        = stack_alloc<T>(&ctx->params_allocator, std::forward<Args>(args)...);
    return bsdfshaders_add(
        &ctx->shaders,
        [](vec3             incident_dir,
           vec3*            scattered_dir,
           uint             n,
           sampled_color_t* out_color,
           vec4*            emission,
           void*            params) -> void {
            T* p = (T*)params;
            p->eval(incident_dir, scattered_dir, n, out_color, emission);
        },
        [](vec3  incident_dir,
           vec2  rand_p,
           vec3* out_scattered_dir,
           void* params) -> void {
            T* p = (T*)params;
            p->sample(incident_dir, rand_p, out_scattered_dir);
        },
        (void*)params);
}