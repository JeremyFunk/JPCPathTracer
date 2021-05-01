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
    uint Size();

  private:
    void set_light(LightResults& out_values, const SurfaceInteraction& interaction, int dest_i, int light_i) const;
    std::vector<Vec3> m_positions;
    std::vector<Spectrum> m_colors;
};

} // namespace jpctracer::shadersys