#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/plugins/integrators/ShadowBehavior.h"

namespace jpctracer
{

struct PathPayload
{
    Spectrum integral;
    Vec2* light_samples;
    Vec2* bsdf_samples;
    uint depth;
};

class PathBehavior : public IRayBehavior<PathPayload>
{
  public:
    PathBehavior(uint light_samples, uint max_depth);

    virtual void ClosestHitProgram(const HitPoint& hit_point, PathPayload* payload, Tracer& trace) const;

    virtual void Miss(const Spectrum& background_color, PathPayload* payload) const;

  private:
    uint m_light_samples;
    uint m_max_depth;
    ShadowBehavior m_shadow_behavior;
};

class PathIntegrator : public IIntegrator
{
  public:
    PathIntegrator(uint light_samples, uint sub_pixels, uint depth);
    void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer, film::Film& film) const;

  private:
    UInt2 m_light_samples;
    uint m_ray_depth;
    uint m_sub_pixels;
    PathBehavior m_ray_behavior;
};
;
} // namespace jpctracer
