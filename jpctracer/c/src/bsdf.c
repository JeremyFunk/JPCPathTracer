#include "bsdf.h"
#include "allocators.h"
#include "jpc_api.h"
#include "sampling.h"
#include "shaders.h"
#include <assert.h>


bsdfnode_t bsdfshaders_add(bsdfshaders_t* shaders, eval_f eval, sample_f sample, void* param)
{
    int id = shaders->shaders_n;
    shaders->evals[id] = eval;
    shaders->samplers[id] = sample;
    shaders->weights[id] = 1;
    shaders->params[id] = param;
    shaders->shaders_n++;
    assert(shaders->shaders_n <= BSDFSHADERSMAX);
    return (bsdfnode_t){id, true};
}

void bsdfshaders_weights(bsdfmixnode_t* nodes, uint nodes_n, float* weights, uint weights_n,
                         stack_allocator_t* allocator)
{
    if (weights_n == 1)
    {
        weights[0] = 1;
        return;
    }
    for (int i = 0; i < weights_n; i++)
        weights[i] = 0;
    size_t used_mem = allocator->used;
    float* nodes_weights = stack_alloc(allocator, sizeof(float) * nodes_n, _Alignof(float));
    for (int i = 0; i < nodes_n; i++)
        nodes_weights[i] = 0;

    nodes_weights[nodes_n - 1] = 1;

    for (int i = nodes_n - 1; i >= 0; i--)
    {
        float left_factor = nodes[i].mix_factor * nodes_weights[i];
        float right_factor = (1 - nodes[i].mix_factor) * nodes_weights[i];

        if (nodes[i].left.is_leaf)
        {
              weights[nodes[i].left.id] += left_factor;
        }
        else
        {
            nodes_weights[nodes[i].left.id] += left_factor;
        }

        if (nodes[i].right.is_leaf)
        {
            weights[nodes[i].right.id] += right_factor;
        }
        else
        {
            nodes_weights[nodes[i].right.id] += right_factor;
        }
    }

    allocator->used = used_mem;
}

void bsdf_evaluate(const materiallib_t* matlib, bidir_scattering_t* scattering, int to_sample_idx, vec2 rand_p,
                   sampled_color_t* out_color, vec4* out_emission)
{
    eval_f evals[BSDFSHADERSMAX];
    sample_f samplers[BSDFSHADERSMAX];
    void* params[BSDFSHADERSMAX];
    float weights[BSDFSHADERSMAX];
    char params_mem[BSDFPARAMSMAX];
    bsdfmixnode_t mixnodes[BSDFMIXNODESMAX];

    bsdfcontext_t ctx;
    ctx.mix_nodes = mixnodes;
    ctx.mix_nodes_n = 0;
    ctx.params_allocator.memory = params;
    ctx.params_allocator.size = BSDFPARAMSMAX;
    ctx.params_allocator.used = 0;
    ctx.shaders.shaders_n = 0;
    ctx.shaders.params = params;
    ctx.shaders.evals = evals;
    ctx.shaders.samplers = samplers;
    ctx.shaders.weights = weights;

    material_t material = matlib->materials[scattering->hit_point.material_id];
    void* shader_param = material_load_params(&material,matlib->textures, scattering->hit_point.uvs,
                                             &ctx.params_allocator);

    create_bsdf_f bsdf_creator = material.bsdf_creator; 
    bsdfnode_t root_node = bsdf_creator(&ctx, shader_param);
    bsdfshaders_weights(ctx.mix_nodes, ctx.mix_nodes_n, ctx.shaders.weights, ctx.shaders.shaders_n,
                        &ctx.params_allocator);

    int sample_indirect_shader = sample_discrete(ctx.shaders.weights, ctx.shaders.shaders_n, &rand_p[0]);
    vec3* out_scattered_dir = scattering->scattered_dirs + to_sample_idx;
    ctx.shaders.samplers[sample_indirect_shader](scattering->incident_dir, rand_p, out_scattered_dir,
                                                 ctx.shaders.params[sample_indirect_shader]);

    for (int i = 0; i < ctx.shaders.shaders_n; i++)
    {
        ctx.shaders.evals[i](scattering, out_color, out_emission, ctx.shaders.params[i]);
    }
}
