#pragma once
#include "allocators.h"
#include "cglm/types.h"
#include "jpc_api.h"
#include "types.h"
#include <stddef.h>


typedef void (*eval_f)(vec3 incident_dir, vec3* scattered_dir, uint n,
                       sampled_color_t* out_color, vec4* emission,
                       void* params);
typedef void (*sample_f)(vec3 incident_dir, vec2 rand_p,
                         vec3* out_scattered_dir, void* params);

/*
bsdf shaders

weight eval_f sample_f type params
 */

typedef struct
{
    uint      count;
    uint      count_max;
    eval_f*   evals;
    sample_f* samplers;
    void**    params;
    float*    weights;
} bsdfshaders_t;

typedef struct bsdfnode_s
{
    int  id;
    bool is_leaf;
} bsdfnode_t;

typedef struct
{
    bsdfnode_t left;
    bsdfnode_t right;
    float      mix_factor;
} bsdfmixnode_t;

typedef struct
{
    uint   bsdf_shaders_max;
    size_t bsdf_params_max;
    uint   bsdf_mixnodes_max;
    uint   bsdf_eval_max;

} bsdf_limits_t;

const static bsdf_limits_t bsdf_default_limits={
    .bsdf_shaders_max = 100,
    .bsdf_params_max = 10000,
    .bsdf_mixnodes_max = 100,
    .bsdf_eval_max = 100,
};

typedef struct bsdfcontext_s
{
    bsdfshaders_t     shaders;
    uint              mix_nodes_count;
    uint              mix_nodes_count_max;
    bsdfmixnode_t*    mix_nodes;
    stack_allocator_t params_allocator;

    hit_point_t      hit;
    vec3             incident_dir;
    sampled_color_t* temp_eval_color;
    uint             eval_color_max;
    mat4             world_to_local;

} bsdfcontext_t;

bsdfnode_t bsdfshaders_add(bsdfshaders_t* shaders, eval_f eval, sample_f sample,
                           void* param);

void bsdfshaders_weights(bsdfmixnode_t* nodes, uint nodes_n, float* weights,
                         uint weights_n, stack_allocator_t* allocator);

bsdfcontext_t* bsdf_alloc(bsdf_limits_t limits);
void           bsdf_free(bsdfcontext_t* bsdf);

void bsdf_init(bsdfcontext_t* bsdf, const materiallib_t* matlib,
               vec3 incident_ray, hit_point_t hit);

void bsdf_sample(bsdfcontext_t* bsdf, vec2 rand_p, vec3* out_scattered);

void bsdf_eval(bsdfcontext_t* bsdf, vec3* scattered_dir, uint n,
               sampled_color_t* out_color, vec4* out_emission);

void bsdf_vec3_to_local(bsdfcontext_t* bsdf, vec3* directions, uint n);
void bsdf_vec3_to_world(bsdfcontext_t* bsdf, vec3* directions, uint n);

bsdfnode_t diffuse(bsdfcontext_t* ctx, float4 color);

bsdfnode_t mix_bsdf(bsdfcontext_t* ctx, bsdfnode_t b1, bsdfnode_t b2,
                    float mix_factor);
