#pragma once
#include "IBXDF.h"
#include "IMicrofacetDistribution.h"
#include "IFresnel.h"

namespace bsdfs
{
    class CookTorranceBSDF : public IBXDF
    {
    public:
        CookTorranceBSDF(const IMicrofacetDistribution* distribution,const IFresnel* fresnel, core::Spectrum reflectance);
        virtual BXDFResult EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point) const override;
        virtual core::SpectrumPasses Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Prec Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Vec3 SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const override;
        virtual core::BxDFType GetType() const override;
        ~CookTorranceBSDF(){}
    private:
        const IMicrofacetDistribution* _distribution;
        const IFresnel* _fresnel;
        core::BxDFType _type;
        core::Spectrum _reflectance;
    };
}