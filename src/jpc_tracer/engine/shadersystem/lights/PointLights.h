#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include <vector>
namespace jpctracer::shadersys
{
class PointLights
{
  public:
    void Add(const Vec3& position, Spectrum color);
    const void Sample(LightResults& out_values, View<Vec2> samples, const SurfaceInteraction& interaction) const;

  private:
    std::vector<Vec3> m_positions;
    std::vector<Spectrum> m_colors;
};

} // namespace jpctracer::shadersys