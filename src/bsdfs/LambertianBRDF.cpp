#include "LambertianBRDF.h"
#include "bsdfs/IBXDF.h"
#include "core/Linalg.h"
#include "core/constants.h"


namespace bsdfs
{
    LambertianBRDF::LambertianBRDF(core::Spectrum reflectance) 
       : _reflectance(reflectance)
    {
        _type.BSDF_REFLECTION = false; 
        _type.BSDF_TRANSMISSION = false;
        _type.BSDF_DIFFUSE = true;
        _type.BSDF_GLOSSY = false;
        _type.BSDF_SPECULAR = false;
    }
    
    BXDFResult LambertianBRDF::EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point) const 
    {
        return BXDFResult();
    }
    
    core::SpectrumPasses LambertianBRDF::Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        return _reflectance / M_PI;
    }
    
    core::Prec LambertianBRDF::Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        return 1;
    }
    
    core::Vec3 LambertianBRDF::SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const 
    {
        return core::Vec3();
    }
    
    core::BxDFType LambertianBRDF::GetType() const 
    {
        return _type;
    }
}