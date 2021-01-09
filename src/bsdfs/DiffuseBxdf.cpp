#include "DiffuseBxdf.h"
#include "core/IBSDF.h"
#include "core/SpectrumPasses.h"
#include "core/constants.h"
#include "samplers/SphereSamplers.h"
#include <iostream>


namespace bsdfs
{
    DiffuseBxdf::DiffuseBxdf(core::Spectrum surface_color) 
        : _surface_color(surface_color)
    {
        
    }
    
    BXDFDirectionInfo DiffuseBxdf::Evaluate(const core::Vec3& incoming_direction,const core::Vec2& random_point,core::BxDFType type)
    {
        
        BXDFDirectionInfo result;
        result.SurfaceColor = _surface_color;
        result.OutgoingDirection = samplers::UniformSampleHemisphere(random_point);
        //std::cout<<result.OutgoingDirection[0]<<std::endl;
        //std::cout<<random_point[1]<<std::endl;
        result.Pdf = 1.0 / (2.0 * M_PI);
        return result;
    }


}