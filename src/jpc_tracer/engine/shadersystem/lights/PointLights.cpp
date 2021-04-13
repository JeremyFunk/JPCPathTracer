#include "PointLights.h"
#include "jpc_tracer/core/maths/maths.h"



namespace jpctracer::shadersys
{

    const RootShader* PointLights::Sample(Ray* out_incident_ray,const SurfaceInteraction& interaction, Vec2 random_point) const
    {
        int size = m_positions.size();
        int sampled_idx = (int) random_point[0]*size;
        
        Vec3 dir = m_positions[sampled_idx] - interaction.Point;

        out_incident_ray->ClipEnd = dir.norm();
        out_incident_ray->Direction = dir.normalized();
        out_incident_ray->LensPosition = {0,0};
        out_incident_ray->Origin = interaction.Point;

        return &m_shaders[sampled_idx];
    }


    
}