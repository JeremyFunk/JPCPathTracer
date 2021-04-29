#include "SunLights.h"
#include "jpc_tracer/core/maths/Constants.h"
#include <limits>

namespace jpctracer::shadersys
{
void SunLights::Add(Vec3 direction, Spectrum color)
{
    direction *= -1; // flip
    m_directions.push_back(direction.normalized());
    m_colors.push_back(color);
}

void SunLights::set_light(LightResults& out_values, const SurfaceInteraction& interaction, int dest_i,
                          int light_i) const
{
    Norm3 dir = m_directions[light_i];
    Ray& out_incident_ray = out_values.rays[dest_i];
    out_incident_ray.ClipEnd = std::numeric_limits<Prec>::max();
    out_incident_ray.Direction = dir;
    out_incident_ray.LensPosition = {0, 0};
    out_incident_ray.Origin = interaction.Point;

    out_values.emission[dest_i].value = m_colors[light_i];
    Transparency(out_values.emission[dest_i].value) = 1;
    out_values.emission[dest_i].pdf = 0;
}

const void SunLights::Sample(LightResults& out_values, View<Vec2> samples, const SurfaceInteraction& interaction) const
{
    if (m_directions.size() == 0)
    {
        out_values.rays.size = 0;
        out_values.emission.size = 0;
    }
    int light_size = m_directions.size();
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

uint SunLights::Size()
{
    return m_directions.size();
}
} // namespace jpctracer::shadersys