#pragma once

#include "core/Linalg.h"
namespace bsdfs
{
    template<class ParamsT>
    class IMicrofacetDistribution
    {
    public:
        
        //the half angle of the outcoming vector
        virtual core::Prec Distribution(const ParamsT* params,const core::Vec3& microfacet_normal) const = 0;
        virtual core::Prec ShadowMasking(const ParamsT* params,const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const = 0;
        virtual core::Vec3 SampleFromDistribution(const ParamsT* params,const core::Vec2 random_point) const = 0;
        virtual core::Prec Pdf(const ParamsT* params,const core::Vec3& scattered_direction,const core::Vec3& micorfacet_normal) const = 0;
        virtual ~IMicrofacetDistribution() = 0;
    };
    template<class ParamsT>
    IMicrofacetDistribution<ParamsT>::~IMicrofacetDistribution()
    {
        
    }
    
    
}