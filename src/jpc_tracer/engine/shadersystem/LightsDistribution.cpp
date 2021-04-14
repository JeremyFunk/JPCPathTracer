#include "LightsDistribution.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include <cstdlib>



namespace jpctracer::shadersys
{
    LightsDistribution::LightsDistribution(const Lights* lights,SurfaceInteraction interaction) 
        : m_point_lights(&lights->point_lights),m_interaction(interaction)
    {
        
    }
    
    std::pair<Spectrum,Prec> LightsDistribution::operator()(const Ray& incident_ray) const
    {
        return {Black(),0};
    }
    
    std::pair<Spectrum,Prec> LightsDistribution::operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
    {
        return {Black(),0};
    }
}