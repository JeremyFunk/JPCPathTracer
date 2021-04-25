#include "DirectLightIntegrator.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/SphericalCoordinates.h"
#include <math.h>

namespace jpctracer
{

DirectLightBehavior::DirectLightBehavior(uint light_samples) : m_light_samples(light_samples)
{
}

DirectLightIntegrator::DirectLightIntegrator(uint sub_pixels, uint light_samples) : direct_light_behavior(0)
{
    m_sub_pixels_x = std::ceil(std::sqrt(sub_pixels));
    m_sub_pixels_y = m_sub_pixels_x;
    m_light_samples_x = std::ceil(std::sqrt(light_samples));
    m_light_samples_y = m_light_samples_x;
    direct_light_behavior = DirectLightBehavior(m_light_samples_x * m_light_samples_y);
}

void DirectLightIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer,
                                      film::Film& film) const
{

    std::vector<Vec2> subpixel_samples(m_sub_pixels_x * m_sub_pixels_y);
    sampler->Sample(UInt2{m_sub_pixels_x, m_sub_pixels_y}, &subpixel_samples[0]);

    std::vector<Vec2> emission_samples(m_sub_pixels_x * m_sub_pixels_y);

    Vec2* emssion_samples_it = &emission_samples[0];

    Spectrum result = FromValue(0);

    int total_light_samples = m_light_samples_x * m_light_samples_y;

    for (const auto& sample : subpixel_samples)
    {
        std::vector<Vec2> integrator_samples(total_light_samples + 1);
        Vec2* samples_it = &integrator_samples[0];
        sampler->Sample(UInt2{m_light_samples_x, m_light_samples_y}, samples_it);
        integrator_samples[total_light_samples] = *emssion_samples_it;
        Payload payload = {samples_it, FromValue(1)};

        Ray ray = camera->Generate(pixel, sample);
        tracer(direct_light_behavior, ray, &payload);
        result += payload.result;

        emssion_samples_it++;
    }

    film.SavePixel("Combined", pixel, result / subpixel_samples.size());
}
void DirectLightBehavior::ClosestHitProgram(const HitPoint& hit_point, Payload* payload, Tracer& tracer) const
{

    auto lights = hit_point.ActiveLights({payload->samples, m_light_samples});
    payload->samples += m_light_samples;

    auto bsdfs = hit_point.Shader(lights.rays);
    // payload->samples+=1;

    for (int i = 0; i < m_light_samples; i++)
    {
        payload->result +=
            ComputeDirectLight({bsdfs.eval_bsdfs[i], bsdfs.eval_pdf[i]}, {lights.emission[i], lights.pdf[i]},
                               lights.rays[i], tracer, m_shadow_behavior);
    }
    payload->result /= m_light_samples;
    payload->result += bsdfs.emission;
}
void DirectLightBehavior::Miss(Spectrum background_color, Payload* payload) const
{
    payload->result = Black();
}

Prec CosWeight(const Norm3& dir)
{
    Prec w = CosTheta(dir);
    return w > 0 ? w : -w;
}

Spectrum ComputeDirectLight(const ShaderResult& bsdf, const ShaderResult& light, Ray& ray, Tracer& tracer,
                            const ShadowBehavior& shadow_behavior)
{

    Payload shadow_test;
    // JPC_LOG_INFO("Shadow ray dir: {},{},{} pos: {},{},{}",
    //    ray.Direction[0], ray.Direction[1], ray.Direction[2],
    //    ray.Origin[0],ray.Origin[1],ray.Origin[2]);
    tracer(shadow_behavior, ray, &shadow_test);
    if (shadow_test.IsShadow)
        return Black();
    auto [light_val, light_pdf] = light;
    auto [bsdf_val, bsdf_pdf] = bsdf;
    if (!IsDeltaDistribution(light_pdf))
        return bsdf_val * light_val / light_pdf * CosWeight(ray.Direction);
    return bsdf_val * light_val * CosWeight(ray.Direction);
};
} // namespace jpctracer