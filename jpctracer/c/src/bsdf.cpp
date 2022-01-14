extern "C"
{
#include "bsdf.h"
#include "cglm/cglm.h"
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"
#include "jpc_api.h"
#include "sampling.h"
#include "shaders.h"
#include "types.h"
#include <assert.h>
}

#include "algorithms.h"
#include "allocators.hpp"
#include <algorithm>

bsdfnode_t bsdfshaders_add(bsdfshaders_t* shaders,
                           eval_f         eval,
                           sample_f       sample,
                           void*          param)
{
    int id = shaders->count;
    shaders->evals[id] = eval;
    shaders->samplers[id] = sample;
    shaders->weights[id] = 1;
    shaders->params[id] = param;
    shaders->count++;
    assert(shaders->count <= shaders->count_max);
    return bsdfnode_t{id, true};
}

void bsdfshaders_weights(bsdfmixnode_t*     nodes,
                         uint               nodes_n,
                         float*             weights,
                         uint               weights_n,
                         stack_allocator_t* allocator)
{
    if (weights_n == 1 || nodes_n == 0)
    {
        weights[0] = 1;
        return;
    }

    size_t used_mem = allocator->used;
    float* nodes_weights = stack_alloc_n<float>(allocator, nodes_n);

    std::fill_n(weights, weights_n, 0);
    std::fill_n(nodes_weights, nodes_n, 0);

    nodes_weights[nodes_n - 1] = 1;

    // precondition:
    //  for all i : nodes[i].left.id < i and nodes[i].right.id < i
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

    allocator->used = used_mem; // free nodes_weights
}

struct bsdf_s
{
    bsdfcontext_t ctx;
    bsdf_limits_t limits;
};

bsdfcontext_t* bsdf_alloc(bsdf_limits_t limits)
{
    return new bsdfcontext_t{
        .shaders = bsdfshaders_t
        {
            .count = 0,
            .count_max = limits.bsdf_shaders_max,
            .evals = new eval_f[limits.bsdf_shaders_max],
            .samplers = new sample_f[limits.bsdf_shaders_max],
            .params = new void*[limits.bsdf_shaders_max],
            .weights = new float[limits.bsdf_shaders_max],
        },
        .mix_nodes_count = 0,
        .mix_nodes_count_max = limits.bsdf_mixnodes_max,
        .mix_nodes = new bsdfmixnode_t[limits.bsdf_mixnodes_max],
        .params_allocator = stack_allocator_t
        {
            .memory = malloc(limits.bsdf_params_max),
            .used = 0,
            .size = limits.bsdf_params_max,
        },
        .temp_eval_color = new sampled_color_t[limits.bsdf_eval_max],
        .eval_color_max = limits.bsdf_eval_max,
   };
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

void normal_transformation(vec3 normal, mat4 dst)
{
    vec4 temp, basis_1, basis_2;
    temp[0] = 0;
    temp[1] = 0;
    temp[2] = 1;
    temp[3] = 0;

    if (fabs(normal[1]) < 0.0001)
    {
        temp[0] = 0;
        temp[1] = -1;
        temp[2] = 0;
        temp[3] = 0;
    }

    glm_vec3_normalize(normal);

    glm_vec3_cross(normal, temp, basis_1);
    glm_vec3_normalize(basis_1);

    glm_vec3_cross(normal, basis_1, basis_2);
    glm_vec3_normalize(basis_2);

    glm_mat4_identity(dst);
    glm_vec4_copy3(basis_1, dst[0]);
    glm_vec4_copy3(basis_2, dst[1]);
    glm_vec4_copy3(normal, dst[2]);

    vec4 test = {0, 0, 1, 0};

    vec4 normal2;
    glm_mat4_mulv(dst, test, normal2);

    vec4 diff = {0, 0, 0, 0};
    glm_vec3_sub(normal, normal2, diff);

    assert(glm_vec4_norm(diff) < 0.001);

    glm_mat4_transpose(dst);

    // Debug
    mat4 dst_inv, actual, expected;
    glm_mat4_transpose_to(dst, dst_inv);

    glm_mat4_mul(dst, dst_inv, actual);

    glm_mat4_identity(expected);

    for (int i = 0; i < 4; i++)
    {
        glm_vec4_sub(actual[i], expected[i], diff);
        assert(glm_vec4_norm(diff) < 0.0001);
    }
}

void bsdf_init(bsdfcontext_t*       ctx,
               const materiallib_t* matlib,
               vec3                 incident_dir,
               hit_point_t          hit)
{

    ctx->hit = hit;
    ctx->shaders.count = 0;
    ctx->mix_nodes_count = 0;
    ctx->params_allocator.used = 0;

    normal_transformation(hit.normal, ctx->world_to_local);
    glm_mat4_mulv3(ctx->world_to_local, incident_dir, 0, ctx->incident_dir);

    glm_vec3_negate(ctx->incident_dir);
    glm_vec3_normalize(ctx->incident_dir);

    material_t material = matlib->materials[hit.material_id];
    void*      shader_param = material_load_params(
        &material, matlib->textures, ctx->hit.uvs, &ctx->params_allocator);

    bsdfnode_t root_node = material.bsdf_creator(ctx, shader_param);

    bsdfshaders_weights(ctx->mix_nodes,
                        ctx->mix_nodes_count,
                        ctx->shaders.weights,
                        ctx->shaders.count,
                        &ctx->params_allocator);
}

void bsdf_sample(bsdfcontext_t* ctx, vec2 rand_p, vec3* out_scattered)
{
    assert(rand_p[0] <= 1. + 1e-6);
    int i = sample_discrete(
        ctx->shaders.weights, ctx->shaders.count, &(rand_p[0]));
    void* param = ctx->shaders.params[i];
    ctx->shaders.samplers[i](ctx->incident_dir, rand_p, out_scattered, param);
}

sampled_color_t operator*(float s, sampled_color_t a)
{
    sampled_color_t result;
    result.pdf = a.pdf * s;
    glm_vec4_scale(a.color, s, result.color);
    return result;
}

sampled_color_t operator*(sampled_color_t a, float s)
{
    return operator*(s, a);
}

sampled_color_t operator+(sampled_color_t a, sampled_color_t b)
{
    sampled_color_t result;
    result.pdf = a.pdf + b.pdf;
    glm_vec4_add(a.color, b.color, result.color);
    return result;
}

void operator+=(sampled_color_t& a, const sampled_color_t& b)
{
    a = a + b;
}

void bsdf_eval(bsdfcontext_t*   ctx,
               vec3*            scattered_dir,
               uint             n,
               sampled_color_t* out_color,
               vec4*            out_emission)
{
    // This is a matrix vector multiplication algorithm
    assert(n <= ctx->eval_color_max);

    std::fill_n(out_color,
                n,
                sampled_color_t{
                    .color = {0, 0, 0, 0},
                    .pdf = 0,
                });
    vec4 temp_emission = {0, 0, 0, 0};
    // E * w = out
    // E in R^m
    for (uint i = 0; i < ctx->shaders.count; i++)
    {
        ctx->shaders.evals[i](ctx->incident_dir,
                              scattered_dir,
                              n,
                              ctx->temp_eval_color,
                              &temp_emission,
                              ctx->shaders.params[i]);

        float weight = ctx->shaders.weights[i];

        glm_vec3_scale(temp_emission, weight, temp_emission);
        glm_vec3_add(temp_emission, *out_emission, *out_emission);
        for (uint j = 0; j < n; j++)
        {
            out_color[j] += weight * ctx->temp_eval_color[j];
        }
    }
}

void bsdf_vec3_to_local(bsdfcontext_t* bsdf, vec3* directions, uint n)
{
    mat_mul_n(bsdf->world_to_local, directions, n, 0);
}
void bsdf_vec3_to_world(bsdfcontext_t* bsdf, vec3* directions, uint n)
{
    mat4 local_to_world;
    glm_mat4_transpose_to(bsdf->world_to_local, local_to_world);
    mat_mul_n(local_to_world, directions, n, 0);
}
