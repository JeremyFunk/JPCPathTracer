#include "bsdf.h"
#include "allocators.h"
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "sampling.h"
#include "shaders.h"
#include <assert.h>



bsdfnode_t bsdfshaders_add(bsdfshaders_t* shaders, eval_f eval, sample_f sample,
                           void* param)
{
    int id = shaders->count;
    shaders->evals[id] = eval;
    shaders->samplers[id] = sample;
    shaders->weights[id] = 1;
    shaders->params[id] = param;
    shaders->count++;
    assert(shaders->count <= shaders->count_max);
    return (bsdfnode_t){id, true};
}

void bsdfshaders_weights(bsdfmixnode_t* nodes, uint nodes_n, float* weights,
                         uint weights_n, stack_allocator_t* allocator)
{
    if (weights_n == 1)
    {
        weights[0] = 1;
        return;
    }
    for (int i = 0; i < weights_n; i++)
        weights[i] = 0;
    size_t used_mem = allocator->used;
    float* nodes_weights
        = stack_alloc(allocator, sizeof(float) * nodes_n, _Alignof(float));
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

struct bsdf_s
{
    bsdfcontext_t ctx;
    bsdf_limits_t limits;
};

bsdfcontext_t* bsdf_alloc(bsdf_limits_t limits)
{
    bsdfcontext_t* ctx = malloc(sizeof(bsdfcontext_t));
    *ctx = (bsdfcontext_t) {
        .mix_nodes = malloc(sizeof(bsdfmixnode_t)*limits.bsdf_mixnodes_max),
        .mix_nodes_count = 0,
        .mix_nodes_count_max = limits.bsdf_mixnodes_max,
        .params_allocator = (stack_allocator_t)
        {
            .memory = malloc(limits.bsdf_params_max),
            .size = limits.bsdf_params_max,
            .used = 0,
        },
        .shaders = (bsdfshaders_t)
        {
            .count = -1,
            .count_max = limits.bsdf_shaders_max,
            .params = malloc(sizeof(void*)*limits.bsdf_shaders_max),
            .evals = malloc(sizeof(eval_f)*limits.bsdf_shaders_max),
            .samplers = malloc(sizeof(sample_f)*limits.bsdf_shaders_max),
            .weights = malloc(sizeof(float)*limits.bsdf_shaders_max),
        },
        .temp_eval_color = malloc(sizeof(sampled_color_t)*limits.bsdf_eval_max),
        .eval_color_max = limits.bsdf_eval_max,
   };
    return ctx;
}
void bsdf_free(bsdfcontext_t* bsdf)
{
    free(bsdf->mix_nodes);
    free(bsdf->params_allocator.memory);
    free(bsdf->shaders.params);
    free(bsdf->shaders.evals);
    free(bsdf->shaders.samplers);
    free(bsdf->shaders.weights);
    free(bsdf);
}

void normal_transformation(vec3 normal,mat4 dst)
{
    vec4 temp,basis_1,basis_2 ;
    temp[0] = 0;
    temp[1] = 0;
    temp[2] = 1;
    temp[3] = 0;

    if( fabs(normal[1])<0.0001)
    {
        temp[0] = 0;
        temp[1] = -1;
        temp[2] = 0;
        temp[3] = 0;
    }

    glm_vec3_normalize(normal);

    glm_vec3_cross(normal,temp,basis_1);
    glm_vec3_normalize(basis_1);

    glm_vec3_cross(normal,basis_1,basis_2);
    glm_vec3_normalize(basis_2);

    glm_mat4_identity(dst);
    glm_vec4_copy(basis_1,dst[0]);
    glm_vec4_copy(basis_2,dst[1]);
    glm_vec4_copy3(normal,dst[2]);
}

void bsdf_init(bsdfcontext_t* ctx, const materiallib_t* matlib,
               vec3 incident_dir, hit_point_t hit)
{

    ctx->hit = hit;

    normal_transformation(hit.normal,ctx->world_to_local);
    glm_mat4_mulv3(ctx->world_to_local,incident_dir,0,ctx->incident_dir);

    glm_vec3_negate(ctx->incident_dir);
    glm_vec3_normalize(ctx->incident_dir);

    material_t material = matlib->materials[hit.material_id];
    void*      shader_param = material_load_params(
        &material, matlib->textures, ctx->hit.uvs, &ctx->params_allocator);

    bsdfnode_t root_node = material.bsdf_creator(ctx, shader_param);

    bsdfshaders_weights(ctx->mix_nodes, ctx->mix_nodes_count,
                        ctx->shaders.weights, ctx->shaders.count,
                        &ctx->params_allocator);

}



void bsdf_sample(bsdfcontext_t* ctx, vec2 rand_p, vec3* out_scattered)
{
    int i
        = sample_discrete(ctx->shaders.weights, ctx->shaders.count, &rand_p[0]);
    void* param = ctx->shaders.params[i];
    ctx->shaders.samplers[i](ctx->incident_dir, rand_p, out_scattered, param);
}

void bsdf_eval(bsdfcontext_t* ctx, vec3* scattered_dir, uint n,
               sampled_color_t* out_color, vec4* out_emission)
{

    assert(n <= ctx->eval_color_max);

    for (int j = 0; j < n; j++)
    {
        out_color[j].color[0] = 0;
        out_color[j].color[1] = 0;
        out_color[j].color[2] = 0;
        out_color[j].color[3] = 0;
        out_color[j].pdf = 0;
    }
    for (int i = 0; i < ctx->shaders.count; i++)
    {
        ctx->shaders.evals[i](ctx->incident_dir, scattered_dir, n,
                              ctx->temp_eval_color, out_emission,
                              ctx->shaders.params[i]);

        for (int j = 0; j < n; j++)
        {
            float weight = ctx->shaders.weights[i];
            glm_vec4_adds(ctx->temp_eval_color[j].color, weight,
                          out_color[j].color);
            out_color[j].pdf += weight * ctx->temp_eval_color[j].pdf;
        }
    }
}

void bsdf_vec3_to_local(bsdfcontext_t* bsdf, vec3* directions, uint n)
{
    for(int i = 0; i<n;i++)
    {
       vec3 temp;
       glm_mat4_mulv3(bsdf->world_to_local,directions[i],0,temp);
       glm_vec3_copy(temp,directions[i]);

    }
}
void bsdf_vec3_to_world(bsdfcontext_t* bsdf, vec3* directions, uint n)
{
    mat4 local_to_world;
    glm_mat4_inv(bsdf->world_to_local,local_to_world);
    for(int i = 0; i<n;i++)
    {
       vec3 temp;
       glm_mat4_mulv3(local_to_world,directions[i],0,temp);
       glm_vec3_copy(temp,directions[i]);

    }
}
