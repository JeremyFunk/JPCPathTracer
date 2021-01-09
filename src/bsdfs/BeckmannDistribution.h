#pragma once

#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
namespace bsdfs
{
    class BeckmannDistribution : public IMicrofacetDistribution
    {
    public:
        BeckmannDistribution(core::Prec roughness);
        virtual core::Prec Distribution(const core::Vec3& half_direction) const override;
        virtual core::Prec ViewedPercentage(const core::Vec3& half_incomeing_direction,const core::Vec3& outgoing_direction ) const override;
        virtual core::Vec3 SampleFromDistribution(const core::Vec3& incoming_direction, const core::Vec2 random_point) const override;
        virtual core::Prec Pdf(const core::Vec3& incoming_direction,const core::Vec3& half_direction) const override;
    private:
        core::Prec SingleViewedPercentage(const core::Vec3& direction) const;
        core::Prec RoughnessToAlpha(core::Prec roughness) const;
        //For the ViewedPercentage
        core::Prec Lambda(const core::Vec3& v) const; 
        core::Prec _alpha2;
    };
}