#pragma once
#include "IMicrofacetDistribution.h"
#include "core/Linalg.h"

namespace bsdfs
{
    struct BeckmannParams
    {
        core::Prec Alpha;
    };

    class BeckmannDistribution final: public IMicrofacetDistribution<BeckmannParams>
    {
    public:
        virtual core::Prec Distribution(const BeckmannParams* params,const core::Vec3& microfacet_normal) const;
        virtual core::Prec ShadowMasking(const BeckmannParams* params,const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const;
        virtual core::Vec3 SampleFromDistribution(const BeckmannParams* params, const core::Vec2 random_point) const;
        virtual core::Prec Pdf(const BeckmannParams* params,const core::Vec3& scattered_direction,const core::Vec3& micorfacet_normal) const;
        virtual core::Prec Lambda(const BeckmannParams* params,const core::Vec3& direction) const;
        virtual ~BeckmannDistribution(){}
    };
}