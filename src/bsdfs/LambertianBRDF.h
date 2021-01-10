#pragma once
#include "IBXDF.h"
#include "core/IBSDF.h"
#include "core/Spectrum.h"

namespace bsdfs
{
    class LambertianBRDF : public IBXDF
    {
    public:
        LambertianBRDF(core::Spectrum reflectance);
        virtual BXDFResult EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point) const override;
        virtual core::SpectrumPasses Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Prec Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Vec3 SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const override;
        virtual core::BxDFType GetType() const override;
    private:
        core::Spectrum _reflectance;
        core::BxDFType _type;
    };
}