#include "LightsDistribution.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <cstdlib>



namespace jpctracer::shadersys
{
    LightsDistribution::LightsDistribution(PointLights* point_lights,SurfaceInteraction interaction) 
        : m_point_lights(point_lights),m_interaction(interaction)
    {
        
    }
    
    std::pair<Spectrum,Prec> LightsDistribution::operator()(const Ray& incident_ray) const
    {
        return {Black(),0};
    }
    
    std::pair<Spectrum,Prec> LightsDistribution::operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
    {
        const RootShader* shader = m_point_lights->Sample(out_incident_ray,m_interaction,random_point);
        return shader->operator()(*out_incident_ray);
    }
}