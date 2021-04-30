#pragma once
#include "Payload.h"
#include "ShadowBehavior.h"
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include <vector>

namespace jpctracer
{
struct DirectLightIntegratorSettings
{
    uint LightSamples;
    uint SubPixels;
};

Prec CosWeight(const Norm3& dir);

template <class T1>
T1 ComputeDirectLight(const Distributed<T1>& bsdf, const Distributed<Spectrum>& light, Ray& ray, Tracer& tracer,
                      const ShadowBehavior& shadow_behavior)
{

    Payload shadow_test;
    // JPC_LOG_INFO("Shadow ray dir: {},{},{} pos: {},{},{}",
    //    ray.Direction[0], ray.Direction[1], ray.Direction[2],
    //    ray.Origin[0],ray.Origin[1],ray.Origin[2]);
    tracer(shadow_behavior, ray, &shadow_test);
    if (shadow_test.IsShadow)
    {
        // JPC_LOG_INFO("In shadow Dir: {} Origin: {}", ray.Direction.to_string(), ray.Origin.to_string());
        // return FromRGB({0, 0, 1});
        return Black();
    }
    auto [light_val, light_pdf] = light;
    auto [bsdf_val, bsdf_pdf] = bsdf;
    if (!IsDeltaDistribution(light_pdf))
        return bsdf_val * light_val / light_pdf * CosWeight(ray.Direction);
    return bsdf_val * light_val * CosWeight(ray.Direction);
};

struct DirectLightBehavior final : public IRayBehavior
{
    DirectLightBehavior(uint light_samples);
    void ClosestHitProgram(const HitPoint& hit_point, Payload* payload, Tracer& tracer) const;

    void Miss(Spectrum background_color, Payload* payload) const;

  private:
    uint m_light_samples;
    ShadowBehavior m_shadow_behavior;
};

class DirectLightIntegrator final : public IIntegrator
{
  public:
    DirectLightIntegrator(uint sub_pixels, uint light_samples);
    void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer, film::Film& film) const;

  private:
    uint m_sub_pixels_x;
    uint m_sub_pixels_y;
    uint m_light_samples_x;
    uint m_light_samples_y;
    DirectLightBehavior direct_light_behavior;
};

} // namespace jpctracer