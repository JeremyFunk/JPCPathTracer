#include "PointLights.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"



namespace jpctracer::shadersys
{
    
    const void PointLights::Sample(View<ShaderResult> out_values, View<Ray> out_incident_rays, View<Vec2> samples,const SurfaceInteraction& interaction) const
    {
        if(m_positions.size()==0)
        {
            for(auto& val:out_values) val = {Black(),0};
            for(auto& ray:out_incident_rays) ray = Ray{{0,0,0},{0,0,0}};
        }
        for(int i = 0;i<samples.size;i++)
        {
            const Vec2& random_point = samples[i];
            int size = m_positions.size();
            int sampled_idx = (int) random_point[0]*size;
            
            Vec3 dir = m_positions[sampled_idx] - interaction.Point;
            Ray& out_incident_ray = out_incident_rays[i];
            out_incident_ray.ClipEnd = dir.norm();
            out_incident_ray.Direction = dir.normalized();
            out_incident_ray.LensPosition = {0,0};
            out_incident_ray.Origin = interaction.Point;

            out_values[i].luminance = m_colors[sampled_idx];
            out_values[i].pdf = 0;
        }
    }

    void PointLights::Add(const Vec3& position, Spectrum color)
    {
        m_positions.push_back(position);
        m_colors.push_back(color);
    }


    
}