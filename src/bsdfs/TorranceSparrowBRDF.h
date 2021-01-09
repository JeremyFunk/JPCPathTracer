#pragma once
#include "IBXDF.h"
#include"IFresnel.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "IMicrofacetDistribution.h"
namespace bsdfs
{
    class TorranceSparrowBRDF : public IBXDF
    {
    public:
        TorranceSparrowBRDF(core::Spectrum basecolor,const IFresnel* fresnel,const IMicrofacetDistribution* distribution);
        virtual BXDFDirectionInfo Evaluate(const core::Vec3& incoming_direction_ns,const core::Vec2& random_point,
            core::BxDFType type = core::BxDFType()) override;
        core::BxDFType Type;
        virtual core::Spectrum SurfaceColor(const core::Vec3& incoming_direction_ns,const core::Vec3& outgoing_direction_ns);
    private:
        const IFresnel* _fresnel;
        core::Spectrum _basecolor;
        const IMicrofacetDistribution* _distribution;
    };
    
}