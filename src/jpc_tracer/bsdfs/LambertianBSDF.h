
#pragma once
#include "core/BsdfGeneric.h"

namespace jpc_tracer  {
    struct LambertianParams
    {
        Spectrum Reflectance;
    };
    class LambertianBSDF final: public BsdfClosureGeneric<LambertianParams>
    {
    public:
        virtual BsdfResult EvaluateAll(const LambertianParams* params, const Vec3& scattered_direction,const Vec2& random_point) const override;
        virtual Spectrum Scattering(const LambertianParams* params, const Vec3& scattered_direction,const Vec3& incident_direction) const override;
        virtual Prec Pdf(const LambertianParams* params, const Vec3& scattered_direction,const Vec3& incident_direction) const override;
        virtual Vec3 SampleIncidentDirection(const LambertianParams* params, const Vec3& scattered_direction, const Vec2& random_point) const override;
    };
}