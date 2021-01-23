#include "LambertianBSDF.h"
#include "bsdfs/SphericalCoordinates.h"
#include "core/Bsdf.h"
#include "samplers/SphereSamplers.h"
#include <iostream>

namespace jpc_tracer
{
    BsdfResult LambertianBSDF::EvaluateAll(const LambertianParams* params, const Vec3& scattered_direction,const Vec2& random_point) const 
    {
        return BsdfResult();
    }
    
    Spectrum LambertianBSDF::Scattering(const LambertianParams* params, const Vec3& scattered_direction,const Vec3& incident_direction) const 
    {
        return params->Reflectance / M_PI;
    }
    
    Prec LambertianBSDF::Pdf(const LambertianParams* params, const Vec3& scattered_direction,const Vec3& incident_direction) const 
    {
        if(SameHemiSphereNormalSpace(scattered_direction, incident_direction))
        {
            float pdf = std::abs(CosTheta(incident_direction)) / M_PI ;
            return pdf;
        }else {

            return 0;
        }
    }
    
    Vec3 LambertianBSDF::SampleIncidentDirection(const LambertianParams* params, const Vec3& scattered_direction, const Vec2& random_point) const 
    {
        //nur abhänging von scattered_direction, wie es gepflippt werden soll
        auto result = CosinusSampleHemisphere(random_point);
        if(SameHemiSphereNormalSpace(result, scattered_direction))
            return result;
        
        return -result;

    }
}