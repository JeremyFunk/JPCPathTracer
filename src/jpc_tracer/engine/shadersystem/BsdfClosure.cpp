#include "BsdfClosure.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include <algorithm>
#include <thread>

namespace jpctracer::shadersys
{

namespace detail
{

struct BsdfLink
{
    struct Node
    {
        int id = -1;
        bool is_leaf = false;
        Node(const BsdfNode& n) : id(n.id), is_leaf(n.is_leaf)
        {
        }
        Node(){};
    };
    Node left;
    Node right;
    Prec left_weight = 1;
    Prec right_weight = 1;
};

struct ShaderContext
{
    // ShaderContext()
    //{
    //    JPC_LOG_INFO("Created ShaderContext");
    //}

    // Settings
    bool should_sample;
    bool should_eval;
    bool is_seperated;

    // State
    BsdfState state = BsdfState::START;
    int bsdf_idx = 0;
    int bsdf_count = 0;
    int links_idx = 0;

    // Inputs
    View<Ray> eval_rays;
    View<Vec2> samples;
    const Ray* scattered_ray;

    // Outputs
    union {
        ShaderResultsCom result_com;
        ShaderResultsSep result_sep;
        ShaderResults<void*> result;
    };

    // Weights
    std::vector<Prec> weights;
    // Links
    std::vector<BsdfLink> links;
    // Ranges
    std::vector<Range> ranges;
    //~ShaderContext()
    //{
    //    JPC_LOG_INFO("Deleted ShaderContext");
    //}
};

thread_local static ShaderContext ctx = ShaderContext{};

void init_context()
{
    ctx.should_sample = false;
    ctx.should_eval = false;
    ctx.is_seperated = false;
    ctx.links_idx = 0;
    ctx.bsdf_count = 0;
    ctx.bsdf_idx = 0;
    ctx.weights.clear();
    ctx.links.clear();
    ctx.ranges.clear();
    // JPC_LOG_INFO("State: {}", ctx.state);
    jpc_assertm(ctx.state == BsdfState::START, "Wrong init state");
}

void clear_bsdf_sep()
{
    // std::fill_n(ctx.all_bsdfs_sep.data, ctx.all_bsdfs_sep.size, LightPasses{});
}

void clear_bsdf_com()
{
    // std::fill_n(ctx.all_bsdfs_com.data, ctx.all_bsdfs_com.size, Black());
}

void __init_context_eval(const Ray& scattered_ray, View<Ray> rays)
{
    init_context();
    ctx.eval_rays = rays;
    ctx.should_eval = true;
    ctx.is_seperated = true;
    ctx.scattered_ray = &scattered_ray;
}

void init_context(ShaderResultsSep& result, const Ray& scattered_ray, View<Ray> rays)
{
    __init_context_eval(scattered_ray, rays);
    ctx.result_sep = result;
}

void init_context(ShaderResultsCom& result, const Ray& scattered_ray, View<Ray> rays)
{
    __init_context_eval(scattered_ray, rays);
    ctx.result_com = result;
}

void init_context(ShaderResultsSep& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples)
{
    init_context(result, scattered_ray, rays);
    ctx.samples = samples;
}

void init_context(ShaderResultsCom& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples)
{
    init_context(result, scattered_ray, rays);
    ctx.samples = samples;
}

Range get_smprange()
{
    return ctx.ranges[ctx.bsdf_idx];
}

Range get_evalrange()
{
    return {0, static_cast<int>(ctx.result.eval_bsdf.size)};
}

Range get_smprays_range()
{
    return {0, static_cast<int>(ctx.result.eval_bsdf.size)};
}

const Vec2* get_samples()
{
    return ctx.samples.data;
}

void set_sampled_direction(Norm3 dir, int idx)
{
    ctx.result.sampled_rays[idx] = *ctx.scattered_ray;
    ctx.result.sampled_rays[idx].Direction = dir;
}

const Ray* get_eval_rays()
{
    return ctx.eval_rays.data;
}

const Ray* get_smp_rays()
{
    return ctx.result.sampled_rays.data;
}

void __accum(MaterialType type, Distributed<Spectrum> eval, int idx, View<Distributed<Passes>> passes,
             View<Distributed<Spectrum>> single)
{
    Prec weight = ctx.weights[ctx.bsdf_idx];

    if (ctx.is_seperated)
    {
        switch (type)
        {
        case MATERIAL_DIFFUSE:
            passes[idx].value.diffuse += eval.value * weight;
            break;
        case MATERIAL_GLOSSY:
            passes[idx].value.glossy += eval.value * weight;
            break;
        case MATERIAL_TRANSMISSION:
            passes[idx].value.transmission += eval.value * weight;
            break;
        case MATERIAL_SUBSURFACE:
            passes[idx].value.subsurface += eval.value * weight;
            break;
        }
        passes[idx].pdf += eval.pdf * weight;
    }
    else
    {
        single[idx].value += eval.value * weight;
        single[idx].pdf += eval.pdf * weight;
    }
}

void accum_eval(MaterialType type, Distributed<Spectrum> eval, int idx)
{
    // JPC_LOG_INFO("Weights Size: {} BsdfIdx: {}", ctx.weights.size(), ctx.bsdf_idx);
    __accum(type, eval, idx, ctx.result_sep.eval_bsdf, ctx.result_com.eval_bsdf);
}

void accum_sampled(MaterialType type, Distributed<Spectrum> eval, int idx)
{
    // JPC_LOG_INFO("Weights Size: {} BsdfIdx: {}", ctx.weights.size(), ctx.bsdf_idx);
    __accum(type, eval, idx, ctx.result_sep.sampled_bsdf, ctx.result_com.sampled_bsdf);
}

bool should_sample()
{
    return ctx.should_sample && ctx.state == BsdfState::SAMPLING;
}

bool should_eval()
{
    return ctx.should_eval && ctx.state == BsdfState::EVALUATION;
}

BsdfNode finalize_bsdf(Spectrum emission, Prec transparency)
{
    int id = ctx.bsdf_idx;
    ctx.bsdf_idx++;
    ctx.weights.push_back(1);
    return BsdfNode{id, true, emission, transparency};
}

void setup_child(BsdfLink::Node node, Prec weight)
{
    if (!node.is_leaf)
    {
        ctx.links[node.id].left_weight *= weight;
        ctx.links[node.id].right_weight *= weight;
    }
    else
    {
        ctx.weights[node.id] *= weight;
    }
}

void compute_weights(BsdfNode root_node)
{
    ctx.bsdf_count = ctx.bsdf_idx;
    if (root_node.is_leaf)
        return;
    for (int bsdf_idx = root_node.id; bsdf_idx >= 0; bsdf_idx--)
    {
        auto& [left, right, left_weight, right_weight] = ctx.links[bsdf_idx];
        setup_child(left, left_weight);
        setup_child(right, right_weight);

        assert(left.is_leaf || left.id < bsdf_idx);
        assert(right.is_leaf || right.id < bsdf_idx);
    }
}

void compute_ranges(View<Vec2> samples)
{
    std::sort(samples.begin(), samples.end(), [](const Vec2& v1, const Vec2& v2) { return v1[0] < v2[0]; });
    Prec last_sum = 0;
    Prec next_sum = 0;

    int samples_idx = 0;
    for (int bsdf_idx = 0; bsdf_idx < ctx.bsdf_count; bsdf_idx++)
    {
        next_sum += ctx.weights[bsdf_idx];
        int last_samples_idx = samples_idx;
        while (next_sum >= samples[samples_idx][0] && samples_idx < samples.size)
        {
            samples[samples_idx][0] -= last_sum;
            samples[samples_idx][0] /= next_sum - last_sum;
            samples_idx++;
        }
        ctx.ranges[last_samples_idx].first = last_samples_idx;
        ctx.ranges[last_samples_idx].last = samples_idx;
        last_sum = next_sum;
    }
}

void finish_context()
{
    ctx.state = BsdfState::START;
}

void next_state(BsdfState state)
{
    ctx.state = state;
    ctx.bsdf_idx = 0;
}

} // namespace detail
BsdfNode MixBsdf(BsdfNode node1, BsdfNode node2, Prec weight)
{
    if (detail::ctx.state == detail::BsdfState::WEIGHTS)
    {

        if (node1.id == -1)
            return node2;
        if (node2.id == -1)
            return node1;

        uint bsdfnode_id = detail::ctx.links_idx;
        detail::ctx.links.push_back(detail::BsdfLink{node1, node2, weight, 1 - weight});
        detail::ctx.links_idx++;
        return {bsdfnode_id, false, node1.emission * weight + node2.emission * (1 - weight),
                node1.transparency * weight + node2.transparency * (1 - weight)};
    }
    return {-1, true};
}
} // namespace jpctracer::shadersys