#pragma once
#include "IFresnel.h"
#include "core/Linalg.h"
namespace jpc_tracer
{
    struct DielectricFresnelParams
    {
        Prec IndexOfRefractionInner = 1.45;
        Prec IndexOfRefractionOuter = 1;
    };
    class DielectricFresnel : public IFresnel<DielectricFresnelParams>
    {
    public:
        virtual Spectrum Evaluate(const DielectricFresnelParams* params, Prec cos_theta_incoming) const override;
    private:

    };
}