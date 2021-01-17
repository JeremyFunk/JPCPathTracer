#pragma once

#include "core/Linalg.h"
namespace bsdfs
{
    
    class IMicrofacetDistribution
    {
    public:
        
        //the half angle of the outcoming vector
        virtual core::Prec Distribution(const core::Vec3& microfacet_normal) const = 0;
        virtual core::Prec ShadowMasking(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const = 0;
        virtual core::Vec3 SampleFromDistribution(const core::Vec3& incoming_direction, const core::Vec2 random_point) const = 0;
        virtual core::Prec Pdf(const core::Vec3& incoming_direction,const core::Vec3& micorfacet_normal) const = 0;
    };
    
}