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

Spectrum ComputeDirectLight(const ShaderResult& bsdf, const ShaderResult& light, Ray& ray, Tracer& tracer,
                            const ShadowBehavior& shadow_behavior);

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