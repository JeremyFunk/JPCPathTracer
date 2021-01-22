#pragma once
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
namespace bsdfs {
    template<class ParamsT>
    class IFresnel
    {
    public:
        virtual core::Spectrum Evaluate(const ParamsT* params, core::Prec cos_theta_incoming) const = 0;
    };

    class FresnelNoOp final: public IFresnel<int>
    {
    public:
        virtual core::Spectrum Evaluate(const int* params, core::Prec cos_theta_incoming) const {return core::Spectrum::FromValue(1);};
    };
}