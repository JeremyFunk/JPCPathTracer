#pragma once

#include "core/Linalg.h"
namespace jpc_tracer
{
    template<class ParamsT>
    class IMicrofacetDistribution
    {
    public:
        
        //the half angle of the outcoming vector
        virtual Prec Distribution(const ParamsT* params,const Vec3& microfacet_normal) const = 0;
        virtual Prec ShadowMasking(const ParamsT* params,const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Vec3 SampleFromDistribution(const ParamsT* params,const Vec2 random_point) const = 0;
        virtual Prec Pdf(const ParamsT* params,const Vec3& scattered_direction,const Vec3& micorfacet_normal) const = 0;
        virtual ~IMicrofacetDistribution() = 0;
    };
    template<class ParamsT>
    IMicrofacetDistribution<ParamsT>::~IMicrofacetDistribution()
    {
        
    }
    
    
}