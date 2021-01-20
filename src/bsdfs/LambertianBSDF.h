
#pragma once
#include "core/BsdfGeneric.h"

namespace bsdfs  {
    struct LambertianParams
    {
        core::Spectrum Reflectance;
    };
    class LambertianBSDF final: public core::BsdfClosureGeneric<LambertianParams>
    {
    public:
        virtual core::BsdfResult EvaluateAll(const LambertianParams* params, const core::Vec3& scattered_direction,const core::Vec2& random_point) const override;
        virtual core::Spectrum Scattering(const LambertianParams* params, const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Prec Pdf(const LambertianParams* params, const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Vec3 SampleIncidentDirection(const LambertianParams* params, const core::Vec3& scattered_direction, const core::Vec2& random_point) const override;
    };
}