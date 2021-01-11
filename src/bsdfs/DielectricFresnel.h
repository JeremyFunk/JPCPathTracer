#pragma once
#include "IFresnel.h"
#include "core/Linalg.h"
namespace bsdfs
{
    class DielectricFresnel : public IFresnel
    {
    public:
        DielectricFresnel(core::Prec index_of_refraction_inner = 1.45,core::Prec index_of_refraction_outer = 1);
        virtual core::Spectrum Evaluate( core::Prec cos_theta_incoming) const override;
    private:
        core::Prec _index_of_refraction_inner;
        core::Prec _index_of_refraction_outer;
    };
}