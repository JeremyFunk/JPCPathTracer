#include "BackgroundDistribution.h"
#include "jpc_tracer/core/maths/Spectrum.h"



namespace jpctracer
{
    std::pair<Spectrum,Prec> BackgroundDistribution::operator()(const Ray& incident_ray) const
    {
        return {Black(),0};
    }
    
    std::pair<Spectrum,Prec> BackgroundDistribution::operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
    {
        return {Black(),0};
    }
    
    std::pair<Spectrum,Prec> BackgroundDistribution::operator()(Ray* out_incident_ray, 
                                                Vec3 random_point) const
    {
        return {Black(),0};
    }
}