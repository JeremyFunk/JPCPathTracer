#include "PointLights.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"

namespace jpctracer::shadersys
{

const void PointLights::Sample(LightResults& out_values, View<Vec2> samples,
                               const SurfaceInteraction& interaction) const
{
    if (m_positions.size() == 0)
    {
        out_values.rays.size = 0;
        out_values.emission.size = 0;
    }
    int light_size = m_positions.size();
    if (samples.size < light_size)
    {
        for (int smp_i = 0; smp_i < samples.size; smp_i++)
        {
            const Vec2& random_point = samples[smp_i];
            int light_i = (int)random_point[0] * light_size;
            set_light(out_values, interaction, smp_i, light_i);
        }
    }
    else
    {
        for (int light_i = 0; light_i < light_size; light_i++)
        {
            set_light(out_values, interaction, light_i, light_i);
        }
        out_values.emission.size = light_size;
        out_values.rays.size = light_size;
    }
}

uint PointLights::Size()
{
    return m_positions.size();
}

void PointLights::set_light(LightResults& out_values, const SurfaceInteraction& interaction, int dest_i,
                            int light_i) const
{
    Vec3 dir = m_positions[light_i] - interaction.Point;
    Ray& out_incident_ray = out_values.rays[dest_i];
    out_incident_ray.ClipEnd = dir.norm();
    out_incident_ray.Direction = dir.normalized();
    out_incident_ray.LensPosition = {0, 0};
    out_incident_ray.Origin = interaction.Point;

    out_values.emission[dest_i].value = m_colors[light_i] / (out_incident_ray.ClipEnd * out_incident_ray.ClipEnd);
    Transparency(out_values.emission[dest_i].value) = 1;
    out_values.emission[dest_i].pdf = 0;
}

void PointLights::Add(const Vec3& position, Spectrum color)
{
    m_positions.push_back(position);
    m_colors.push_back(color);
}

} // namespace jpctracer::shadersys