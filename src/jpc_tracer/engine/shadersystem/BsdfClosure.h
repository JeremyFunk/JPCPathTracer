#pragma once
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include <functional>
#include <iostream>

#include <cassert>
#include <spdlog/spdlog.h>

#define assertm(exp, msg) assert(((void)msg, exp))

namespace jpctracer::shadersys
{

struct ShaderResult
{
    Spectrum luminance;
    Prec pdf;
};

struct IBsdfClosure
{
    virtual ShaderResult Eval(Ray incident_ray) const = 0;
    virtual Norm3 Sample2D(Vec2 rand_p) const
    {
        return Norm3{};
    }
    virtual Spectrum Emission() const
    {
        return Black();
    }
    virtual Prec Transparency() const
    {
        return 1;
    }
};

struct BsdfNode
{
    const int id = -1;
    const bool is_leaf = false;
    Spectrum emission = Black();
    Prec transparency = 1;
};

template <class T> concept Closure = requires(T f, Ray scattered)
{
    // clang-format off
    {f(scattered)} ->std::convertible_to<BsdfNode>;
    // clang-format on
};

template <std::derived_from<IBsdfClosure> T> BsdfNode __CreateBsdf(MaterialType type, const T& bsdf);

BsdfNode MixBsdf(BsdfNode node1, BsdfNode node2, Prec weight);

void EvalShader(const Closure auto& shader, Ray scattered_ray, View<Ray> rays, SeperatedBsdfs& result);
void EvalShader(const Closure auto& shader, Ray scattered_ray, View<Ray> rays, CombinedBsdfs& result);

void SampleShader(const Closure auto& shader, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                  SeperatedBsdfs& result);
void SampleShader(const Closure auto& shader, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                  CombinedBsdfs& result);

Spectrum EmissionShader(const Closure auto& shader);
Prec TransparencyShader(const Closure auto& shader);

/******************************************************************************/
/*                  Implementation                                            */
/******************************************************************************/

namespace detail
{

struct Range
{
    int first;
    int last;
};

enum class BsdfState
{
    START = 0,
    WEIGHTS = 1,
    SAMPLING = 2,
    EVALUATION = 3,
    END = 4
};

void init_context();
void init_context(SeperatedBsdfs& result, const Ray& scattered_ray, View<Ray> rays);
void init_context(CombinedBsdfs& result, const Ray& scattered_ray, View<Ray> rays);
void init_context(SeperatedBsdfs& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples);
void init_context(CombinedBsdfs& result, const Ray& scattered_ray, View<Ray> rays, View<Vec2> samples);

Range get_smprange();
Range get_evalrange();

const Vec2* get_samples();

void set_sampled_direction(Norm3 dir, int idx);
const Ray* get_eval_rays();
void accum_eval(MaterialType type, ShaderResult eval, int idx);


bool should_sample();
bool should_eval();

BsdfNode finalize_bsdf(Spectrum emission, Prec transparency);

void compute_weights(BsdfNode root_node);

void compute_ranges(View<Vec2> samples);

void finish_context();

void next_state(BsdfState state);

template <class T> void sample_bsdf(const T& bsdf)
{
    Range range = get_smprange();

    const Vec2* samples = get_samples();
    for (int i = range.first; i < range.last; i++)
        set_sampled_direction(bsdf.Sample2D(samples[i]), i);
}

template <class T> void eval_bsdf(MaterialType type, const T& bsdf)
{
    Range range = get_evalrange();
    const Ray* rays = get_eval_rays();

    for (int i = range.first; i < range.last; i++)
        accum_eval(type, bsdf.Eval(rays[i]), i);
}

template<Closure S, class R>
void EvalShader(S&& shader, Ray scattered_ray, View<Ray> rays, R& result)
{
    init_context(result, scattered_ray, rays);
    for (int i = 1; i < 4; i += 2)
    {
        next_state((BsdfState)i);
        BsdfNode root = shader(scattered_ray);
        if (i == 1)
            compute_weights(root);
    }
    finish_context();
}
template <Closure S, class R>
void SampleShader(S&& shader, Ray scattered_ray, View<Ray> rays, View<Vec2> samples, R& result)
{
    init_context(result, scattered_ray, rays, samples);
    for (int i = 1; i < 4; i += 2)
    {
        next_state((BsdfState)i);
        BsdfNode root = shader(scattered_ray);
        if (i == 1)
        {
            result.emission = root.emission;
            result.emission = root.emission;

            compute_weights(root);
        }
    }

    finish_context();
}

} // namespace detail

template <std::derived_from<IBsdfClosure> T> BsdfNode __CreateBsdf(MaterialType type, const T& bsdf)
{
    if (detail::should_sample())
        detail::sample_bsdf(bsdf);
    if (detail::should_eval())
        detail::eval_bsdf(type, bsdf);
    return detail::finalize_bsdf(bsdf.Emission(), bsdf.Transparency());
}
template <Closure S>
void EvalShader(S&& shader, Ray scattered_ray, View<Ray> rays, SeperatedBsdfs& result)
{
    detail::EvalShader(shader, scattered_ray, rays, result);
}
template <Closure S> 
void EvalShader(S&& shader, Ray scattered_ray, View<Ray> rays, CombinedBsdfs& result)
{
    detail::EvalShader(shader, scattered_ray, rays, result);
}
template <Closure S>
void SampleShader(S&& shader, Ray scattered_ray, View<Ray> rays, View<Vec2> samples, SeperatedBsdfs& result)
{
    detail::SampleShader(shader, scattered_ray, rays, samples, result);
}
template <Closure S>
void SampleShader(S&& shader, Ray scattered_ray, View<Ray> rays, View<Vec2> samples, CombinedBsdfs& result)
{
    detail::SampleShader(shader, scattered_ray, rays, samples, result);
}
template <Closure S> 
Spectrum EmissionShader(S&& shader)
{
    detail::init_context();
    next_state(detail::BsdfState::WEIGHTS);
    BsdfNode node = shader(Ray{});
    detail::finish_context();
    return node.emission;
}
template <Closure S> 
Prec TransparencyShader(S&& shader)
{
    detail::init_context();
    next_state(detail::BsdfState::WEIGHTS);
    BsdfNode node = shader(Ray{});
    detail::finish_context();
    return node.transparency;
}

} // namespace jpctracer::shadersys
