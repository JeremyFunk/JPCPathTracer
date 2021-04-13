#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "NormalSpace.h"

namespace jpctracer::shadersys {
    class IBsdfClosure
    {
    public:
        virtual std::pair<Spectrum,Prec> operator()(const NormalSpace& context, 
                                Ray incident_ray) const = 0;
        virtual std::pair<Spectrum,Prec> operator()(const NormalSpace& context, 
                                Ray* out_incident_ray,Vec2 rand_p) const
        {
            return {Black(),0};
        }
        virtual std::pair<Spectrum,Prec> operator()(const NormalSpace& context, 
                                Ray* out_incident_ray,Vec3 rand_p) const
        {
            return {Black(),0};
        }
    };
}