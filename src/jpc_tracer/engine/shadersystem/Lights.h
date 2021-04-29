#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include "jpc_tracer/engine/shadersystem/lights/PointLights.h"
#include "jpc_tracer/engine/shadersystem/lights/SunLights.h"
#include <vector>

namespace jpctracer::shadersys
{

class Lights
{
  public:
    void AddPointLight(const Vec3& position, Spectrum color);
    void AddDistanceLight(const Vec3& direction, Spectrum color);
    // sampled_rays in world space
    const void Sample(View<Vec2> samples, const SurfaceInteraction& interaction, LightResults& result) const;

  private:
    static constexpr uint light_types = 2;
    void compute_weights();
    PointLights point_lights;
    SunLights sun_lights;
    Prec weights[light_types];
};

} // namespace jpctracer::shadersys