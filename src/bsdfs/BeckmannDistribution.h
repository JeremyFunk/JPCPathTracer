#pragma once

#include "IMicrofacetDistribution.h"
#include "core/Linalg.h"

namespace bsdfs
{
    class BeckmannDistribution : public IMicrofacetDistribution
    {
    public:
        BeckmannDistribution(core::Prec alpha);
        virtual core::Prec Distribution(const core::Vec3& microfacet_normal) const override;
        virtual core::Prec ShadowMasking(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Vec3 SampleFromDistribution(const core::Vec3& incoming_direction, const core::Vec2 random_point) const override;
        virtual core::Prec Pdf(const core::Vec3& incoming_direction,const core::Vec3& micorfacet_normal) const override;
        static core::Prec RoughnessToAlpha(core::Prec roughness);
        core::Prec Lambda(const core::Vec3& x) const;
    private:
        core::Prec _alpha = 0;
        

    };
}