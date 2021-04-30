#include "Lights.h"
namespace jpctracer::shadersys
{
void Lights::AddPointLight(const Vec3& position, Spectrum color)
{
    point_lights.Add(position, color);
}

const void Lights::Sample(View<Vec2> samples, const SurfaceInteraction& interaction, LightResults& result) const
{
    point_lights.Sample(result, samples, interaction);
}
} // namespace jpctracer::shadersys