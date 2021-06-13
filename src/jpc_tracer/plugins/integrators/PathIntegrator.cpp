#include "PathIntegrator.h"
#include "DirectLightIntegrator.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{

PathIntegrator::PathIntegrator(uint light_samples, uint sub_pixels, uint depth)
    : m_light_samples{(uint)std::sqrt(light_samples), (uint)std::sqrt(light_samples)}, m_ray_depth(depth),
      m_sub_pixels(sub_pixels), m_ray_behavior(light_samples, depth)

{
}
void PathIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer,
                               film::Film& film) const
{
    int total_light_samples = m_light_samples[0] * m_light_samples[0];

    std::vector<Vec2> subpixel_samples = create_samples(sampler, {m_sub_pixels, m_sub_pixels});
    Spectrum result = FromValue(0);

    for (const auto& sample : subpixel_samples)
    {
        std::vector<Vec2> light_samples(total_light_samples * m_ray_depth);
        std::vector<Vec2> bsdf_samples(m_ray_depth);
        for (uint depth = 0; depth < m_ray_depth; depth++)
        {
            sampler->Sample(m_light_samples, &light_samples[depth * total_light_samples]);
            sampler->Sample({1, 1}, &bsdf_samples[depth]);
        }
        PathPayload payload = {FromValue(0), light_samples.data(), bsdf_samples.data(), (uint)0};
        Ray ray = camera->Generate(pixel, sample);
        tracer(m_ray_behavior, ray, &payload);
        result += payload.integral;
    }

    film.SavePixel("Combined", pixel, result / subpixel_samples.size());
}
PathBehavior::PathBehavior(uint light_samples, uint max_depth) : m_light_samples(light_samples), m_max_depth(max_depth)
{
}
void PathBehavior::ClosestHitProgram(const HitPoint& hit_point, PathPayload* payload, Tracer& trace) const
{
    auto lights = hit_point.ActiveLights(View<Vec2>{payload->light_samples, m_light_samples});
    payload->light_samples += m_light_samples;
    auto bsdfs = hit_point.Shader(lights.rays, View<Vec2>{payload->bsdf_samples, 1});
    payload->bsdf_samples++;

    Spectrum result = FromValue(0);
    for (int i = 0; i < lights.rays.size; i++)
    {
        result += ComputeDirectLight(bsdfs.eval_bsdf[i], lights.emission[i], lights.rays[i], trace, m_shadow_behavior);
    }
    result += bsdfs.emission;
    // JPC_LOG_INFO("LightResult {}",result.to_string());

    payload->depth++;
    if (payload->depth < m_max_depth)
    {
        auto sampled_bsdf = bsdfs.sampled_bsdf[0];
        trace(*this, bsdfs.sampled_rays[0], payload);
        if (IsDeltaDistribution(sampled_bsdf.pdf))
            result += payload->integral * sampled_bsdf.value;
        else
            result += payload->integral * sampled_bsdf.value / sampled_bsdf.pdf;
    }
    payload->integral = result;
}

void PathBehavior::Miss(const Spectrum& background_color, PathPayload* payload) const
{
    payload->integral = background_color;
}
} // namespace jpctracer
