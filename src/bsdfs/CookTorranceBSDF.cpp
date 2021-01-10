#include "CookTorranceBSDF.h"
#include "bsdfs/IBXDF.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include "MicrofacetHelper.h"


namespace bsdfs
{
    CookTorranceBSDF::CookTorranceBSDF(const IMicrofacetDistribution* distribution,const IFresnel* fresnel) 
        : _distribution(distribution), _fresnel(fresnel)
    {
        _type.BSDF_REFLECTION = false; 
        _type.BSDF_TRANSMISSION = false;
        _type.BSDF_DIFFUSE = true;
        _type.BSDF_GLOSSY = true;
        _type.BSDF_SPECULAR = false;
    }
    
    BXDFResult CookTorranceBSDF::EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point) const 
    {
        return BXDFResult();
    }
    
    core::SpectrumPasses CookTorranceBSDF::Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        //See Microfacet Models for Refraction through Rough Surfaces 2007 (20)
        //n is in the normal space (0,0,1)
        core::Vec3 microfacet_normal = MicrofacetNormal(scattered_direction, incident_direction);
        core::Prec cos_theta_scatterd_micro = std::abs(scattered_direction.dot(microfacet_normal));
        core::SpectrumPasses fresnel = _fresnel->Evaluate(cos_theta_scatterd_micro);
        core::Prec distribution = _distribution->Distribution(microfacet_normal);
        core::Prec shadowing = _distribution->ShadowMasking(scattered_direction,incident_direction);
        core::Prec cos_theta_scatterd_incident = std::abs(scattered_direction[2]*incident_direction[2]);

        core::SpectrumPasses result = fresnel*shadowing*distribution / (4 * cos_theta_scatterd_incident);
        auto rgb = result.GetCombined().ToRGB();
        if (rgb[0] == 0 && rgb[1] == 0 && rgb[2] == 0)
        {
            return result;
        }
        return result;

    }
    
    core::Prec CookTorranceBSDF::Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        return 1;
    }
    
    core::Vec3 CookTorranceBSDF::SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const 
    {
        return core::Vec3();
    }
    
    core::BxDFType CookTorranceBSDF::GetType() const 
    {
        return _type;
    }
    
}