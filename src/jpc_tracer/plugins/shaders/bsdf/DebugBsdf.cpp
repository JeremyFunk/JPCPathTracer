#include "DebugBsdf.h"
#include "jpc_tracer/core/maths/Spectrum.h"



namespace jpctracer
{

    DebugBsdfClosure::DebugBsdfClosure(Spectrum color)
    :m_color(color)
    {

    }

    std::pair<Spectrum,Prec> DebugBsdfClosure::operator()(const NormalSpace& context, 
                                    Ray incident_ray) const
    {
        return {m_color,1};
    }
}