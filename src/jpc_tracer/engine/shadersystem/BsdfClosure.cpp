#include "BsdfClosure.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
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
    View<Ray> sampled_rays;
    View<Prec> pdf;
    union {
        View<Spectrum> all_bsdfs_com;
        View<LightPasses> all_bsdfs_sep;
    };
    union {
        View<Spectrum> eval_bsdfs_com;
        View<LightPasses> eval_bsdfs_sep;
    };
    union {
        View<Spectrum> sampled_bsdfs_com;
        View<LightPasses> sampled_bsdfs_sep;
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

void init_context(SeperatedBsdfs& result, const Ray& scattered_ray, View<Ray> rays)
{
    init_context();
    ctx.eval_rays = rays;
    ctx.should_eval = true;
    ctx.is_seperated = true;
    ctx.all_bsdfs_sep = result.all_bsdfs;
    ctx.eval_bsdfs_sep = result.eval_bsdfs;
    clear_bsdf_sep();
    ctx.scattered_ray = &scattered_ray;
    ctx.pdf = result.all_pdf;
}

void init_context(CombinedBsdfs& result, const Ray& scattered_ray, View<Ray> rays)
{
    init_context();
    ctx.eval_rays = rays;
    ctx.should_eval = true;
    ctx.is_seperated = false;
    ctx.all_bsdfs_com = result.all_bsdfs;
    ctx.eval_bsdfs_com = result.eval_bsdfs;
    clear_bsdf_com();
    ctx.scattered_ray = &scattered_ray;
    ctx.pdf = result.all_pdf;
}

void init_context(SeperatedBsdfs& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples)
{
    init_context(result, scattered_ray, rays);
    ctx.sampled_bsdfs_sep = result.sampled_bsdfs;
    ctx.sampled_rays = result.sampled_rays;
}

void init_context(CombinedBsdfs& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples)
{
    init_context(result, scattered_ray, rays);
    ctx.sampled_bsdfs_com = result.sampled_bsdfs;
    ctx.sampled_rays = result.sampled_rays;
}

Range get_smprange()
{
    return ctx.ranges[ctx.bsdf_idx];
}

Range get_evalrange()
{
    return {0, static_cast<int>(ctx.is_seperated ? ctx.eval_bsdfs_com.size : ctx.eval_bsdfs_com.size)};
}

Range get_smprays_range()
{
    return {0, static_cast<int>(ctx.is_seperated ? ctx.sampled_bsdfs_com.size : ctx.sampled_bsdfs_com.size)};
}

const Vec2* get_samples()
{
    return ctx.samples.data;
}

void set_sampled_direction(Norm3 dir, int idx)
{
    ctx.sampled_rays[idx] = *ctx.scattered_ray;
    ctx.sampled_rays[idx].Direction = dir;
}

const Ray* get_eval_rays()
{
    return ctx.eval_rays.data;
}

const Ray* get_smp_rays()
{
    return ctx.sampled_rays.data;
}

void accum_eval(MaterialType type, ShaderResult eval, int idx)
{
    // JPC_LOG_INFO("Weights Size: {} BsdfIdx: {}", ctx.weights.size(), ctx.bsdf_idx);
    Prec weight = ctx.weights[ctx.bsdf_idx];
    ctx.pdf[idx] += eval.pdf * weight;
    if (ctx.is_seperated)
    {
        switch (type)
        {
        case MATERIAL_DIFFUSE:
            ctx.eval_bsdfs_sep[idx].diffuse += eval.luminance * weight;
            break;
        case MATERIAL_GLOSSY:
            ctx.eval_bsdfs_sep[idx].glossy += eval.luminance * weight;
            break;
        case MATERIAL_TRANSMISSION:
            ctx.eval_bsdfs_sep[idx].transmission += eval.luminance * weight;
            break;
        case MATERIAL_SUBSURFACE:
            ctx.eval_bsdfs_sep[idx].subsurface += eval.luminance * weight;
            break;
        }
    }
    else
    {
        ctx.eval_bsdfs_com[idx] += eval.luminance * weight;
    }
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