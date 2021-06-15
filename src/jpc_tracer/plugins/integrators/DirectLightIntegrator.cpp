#include "DirectLightIntegrator.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/SphericalCoordinates.h"
#include <math.h>
#include <vector>

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

std::vector<Vec2> create_samples(ISampler* sampler, UInt2 counts)
{
    std::vector<Vec2> result(counts[0] * counts[1]);
    sampler->Sample(counts, result.data());
    return result;
}

void DirectLightIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer,
                                      film::Film& film) const
{
    int total_light_samples = m_light_samples_x * m_light_samples_y;

    std::vector<Vec2> subpixel_samples = create_samples(sampler, {m_sub_pixels_x, m_sub_pixels_y});
    Spectrum result = FromValue(0);

    for (const auto& sample : subpixel_samples)
    {
        std::vector<Vec2> light_samples = create_samples(sampler, {m_light_samples_x, m_light_samples_y});

        Payload payload = {light_samples.data(), FromValue(1)};
        Ray ray = camera->Generate(pixel, sample);
        tracer(direct_light_behavior, ray, &payload);
        result += payload.result;
    }

    film.SavePixel("Combined", pixel, result / subpixel_samples.size());
}
void DirectLightBehavior::ClosestHitProgram(const HitPoint& hit_point, Payload* payload, Tracer& tracer) const
{

    auto lights = hit_point.ActiveLights({payload->samples, m_light_samples});
    payload->samples += m_light_samples;

    auto bsdfs = hit_point.Shader(lights.rays);

    // payload->samples+=1;
    // JPC_LOG_INFO("Size: {}", lights.rays.size);
    for (int i = 0; i < lights.rays.size; i++)
    {
        payload->result +=
            ComputeDirectLight(bsdfs.eval_bsdf[i], lights.emission[i], lights.rays[i], tracer, m_shadow_behavior);
    }

    payload->result += bsdfs.emission;
}
void DirectLightBehavior::Miss(Spectrum background_color, Payload* payload) const
{
    payload->result = Black();
}

Prec CosWeight(const Norm3& dir)
{
    Prec w = CosTheta(dir);
    return w > 0 ? w : 0;
}

} // namespace jpctracer