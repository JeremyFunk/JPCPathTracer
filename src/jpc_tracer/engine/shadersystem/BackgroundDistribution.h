#pragma once
#include "jpc_tracer/core/core.h"

namespace jpctracer
{
    class BackgroundDistribution
    {
    public:
        std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec2 random_point) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec3 random_point) const;
    };
}