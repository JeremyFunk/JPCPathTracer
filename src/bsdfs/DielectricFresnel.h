#pragma once
#include "IFresnel.h"
#include "core/Linalg.h"
namespace bsdfs
{
    struct DielectricFresnelParams
    {
        core::Prec IndexOfRefractionInner = 1.45;
        core::Prec IndexOfRefractionOuter = 1;
    };
    class DielectricFresnel : public IFresnel<DielectricFresnelParams>
    {
    public:
        virtual core::Spectrum Evaluate(const DielectricFresnelParams* params, core::Prec cos_theta_incoming) const override;
    private:

    };
}