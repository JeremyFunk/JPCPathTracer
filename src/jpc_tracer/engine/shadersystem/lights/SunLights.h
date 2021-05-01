#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include <vector>
namespace jpctracer::shadersys
{
class SunLights
{
  public:
    void Add(Vec3 direction, Spectrum color);
    const void Sample(LightResults& out_values, View<Vec2> samples, const SurfaceInteraction& interaction) const;
    uint Size();

  private:
    void set_light(LightResults& out_values, const SurfaceInteraction& interaction, int dest_i, int light_i) const;
    std::vector<Norm3> m_directions;
    std::vector<Spectrum> m_colors;
};
} // namespace jpctracer::shadersys