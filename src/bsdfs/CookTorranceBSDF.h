#pragma once
#include "bsdfs/IFresnel.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "bsdfs/SphericalCoordinates.h"
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include <memory>
namespace jpc_tracer
{
    template<class FresnelParamsT,class DistributionParamsT>
    struct CookTorranceParams
    {
        FresnelParamsT Fresnel;
        DistributionParamsT Distribution;
        Spectrum Reflectance;
    };

    template<class FresnelParamsT,class DistributionParamsT>
    class CookTorranceBSDFClosure final: public BsdfClosureGeneric<CookTorranceParams<FresnelParamsT,DistributionParamsT>>
    {
    public:
        CookTorranceBSDFClosure(std::shared_ptr<IFresnel<FresnelParamsT>> fresnel, std::shared_ptr<IMicrofacetDistribution<DistributionParamsT>> distribution);
        using ParamsT = CookTorranceParams<FresnelParamsT,DistributionParamsT>;
        virtual BsdfResult EvaluateAll(const ParamsT* params, const Vec3& scattered_direction,const Vec2& random_point) const;
        virtual Spectrum Scattering(const ParamsT* params, const Vec3& scattered_direction,const Vec3& incident_direction) const;
        virtual Prec Pdf(const ParamsT* params, const Vec3& scattered_direction,const Vec3& incident_direction) const;
        virtual Vec3 SampleIncidentDirection(const ParamsT* params, const Vec3& scattered_direction, const Vec2& random_point) const;
    private:
        std::shared_ptr<IFresnel<FresnelParamsT>> _fresnel;
        std::shared_ptr<IMicrofacetDistribution<DistributionParamsT>> _distribution;
    };
    template<class FresnelParamsT, class DistributionParamsT>
    CookTorranceBSDFClosure<FresnelParamsT, DistributionParamsT>::CookTorranceBSDFClosure(std::shared_ptr<IFresnel<FresnelParamsT>> fresnel, std::shared_ptr<IMicrofacetDistribution<DistributionParamsT>> distribution) 
        : _distribution(distribution), _fresnel(fresnel)
    {
        
    }
    
    template<class FresnelParamsT, class DistributionParamsT>
    BsdfResult CookTorranceBSDFClosure<FresnelParamsT, DistributionParamsT>::EvaluateAll(const ParamsT* params, const Vec3& scattered_direction,const Vec2& random_point) const
    {
        return BsdfResult();
    }
    



    template<class FresnelParamsT, class DistributionParamsT>
    Spectrum CookTorranceBSDFClosure<FresnelParamsT, DistributionParamsT>::Scattering(const CookTorranceParams<FresnelParamsT,DistributionParamsT>* params, 
    const Vec3& scattered_direction,const Vec3& incident_direction) const
    {
            Prec cos_scattered = CosTheta(scattered_direction);
            Prec cos_incident = CosTheta(incident_direction);
            Vec3 microfacet_normal = scattered_direction+incident_direction;
            microfacet_normal = microfacet_normal.normalized();
            Prec D = _distribution->Distribution(&params->Distribution,microfacet_normal);
            Prec G = _distribution->ShadowMasking(&params->Distribution,scattered_direction,incident_direction);
            Spectrum F =_fresnel->Evaluate(&params->Fresnel,CosTheta(scattered_direction));
            return params->Reflectance*F * D*G / (4*cos_scattered*cos_incident);
    }
    
    template<class FresnelParamsT, class DistributionParamsT>
    Prec CookTorranceBSDFClosure<FresnelParamsT, DistributionParamsT>::Pdf(const CookTorranceParams<FresnelParamsT,DistributionParamsT>* params, 
    const Vec3& scattered_direction,const Vec3& incident_direction) const
    {
        Vec3 microfacet_normal = scattered_direction+incident_direction;
        microfacet_normal = microfacet_normal.normalized();
        return _distribution->Pdf(&params->Distribution,scattered_direction,microfacet_normal) / (4*microfacet_normal.dot(scattered_direction)) ;
    }
    
    template<class FresnelParamsT, class DistributionParamsT>
    Vec3 CookTorranceBSDFClosure<FresnelParamsT, DistributionParamsT>::SampleIncidentDirection(const CookTorranceParams<FresnelParamsT,DistributionParamsT>* params, 
    const Vec3& scattered_direction, const Vec2& random_point) const
    {
        Vec3 microfacet_normal = _distribution->SampleFromDistribution(&params->Distribution,random_point);

        Vec3 incident_direction = Reflect(scattered_direction, microfacet_normal);

        if(! SameHemiSphereNormalSpace(incident_direction, scattered_direction))
            return -incident_direction;
        return incident_direction;
    }
    


}