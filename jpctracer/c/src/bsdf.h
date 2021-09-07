#pragma once
#include "allocators.h"
#include "cglm/types.h"
#include "jpc_api.h"
#include "types.h"
#include <stddef.h>

typedef struct
{
    vec4 location;
    int  material_id;
    vec2 uvs;
    vec3 normal;
} hit_point_t;
typedef struct
{
    float pdf;
    vec3  color;
} sampled_color_t;
typedef struct
{
    vec3        incident_dir;
    vec3*       scattered_dirs;
    int         scattered_n;
    hit_point_t hit_point;

} bidir_scattering_t;

typedef void (*eval_f)(const bidir_scattering_t* scattering, sampled_color_t* out_color, vec4* emission, void* params);
typedef void (*sample_f)(vec3 incident_dir, vec2 rand_p, vec3* out_scattered_dir, void* params);

/*
bsdf shaders

weight eval_f sample_f type params
 */

#define BSDFSHADERSMAX 100
#define BSDFPARAMSMAX 10000
#define BSDFMIXNODESMAX 100

typedef struct
{
    uint      shaders_n;
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

typedef struct bsdfcontext_s
{
    bsdfshaders_t     shaders;
    uint              mix_nodes_n;
    bsdfmixnode_t*    mix_nodes;
    stack_allocator_t params_allocator;
} bsdfcontext_t;

bsdfnode_t bsdfshaders_add(bsdfshaders_t* shaders, eval_f eval, sample_f sample, void* param);

void bsdfshaders_weights(bsdfmixnode_t* nodes, uint nodes_n, float* weights, uint weights_n,
                         stack_allocator_t* allocator);

// works inplace on scattered dir
void bsdf_evaluate(const materiallib_t* matlib, bidir_scattering_t* scattering, int to_sample_idx, vec2 rand_p,
                   sampled_color_t* out_color, vec4* out_emission);

bsdfnode_t diffuse(bsdfcontext_t* ctx, float4 color);

bsdfnode_t mix_bsdf(bsdfcontext_t* ctx, bsdfnode_t b1, bsdfnode_t b2, float mix_factor);
