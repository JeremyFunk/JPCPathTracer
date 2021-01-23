#pragma once
#include "IMicrofacetDistribution.h"
#include "core/Linalg.h"

namespace jpc_tracer
{
    struct BeckmannParams
    {
        Prec Alpha;
    };

    class BeckmannDistribution final: public IMicrofacetDistribution<BeckmannParams>
    {
    public:
        virtual Prec Distribution(const BeckmannParams* params,const Vec3& microfacet_normal) const;
        virtual Prec ShadowMasking(const BeckmannParams* params,const Vec3& scattered_direction,const Vec3& incident_direction) const;
        virtual Vec3 SampleFromDistribution(const BeckmannParams* params, const Vec2 random_point) const;
        virtual Prec Pdf(const BeckmannParams* params,const Vec3& scattered_direction,const Vec3& micorfacet_normal) const;
        virtual Prec Lambda(const BeckmannParams* params,const Vec3& direction) const;
        virtual ~BeckmannDistribution(){}
    };
}