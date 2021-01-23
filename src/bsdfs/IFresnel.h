#pragma once
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
namespace jpc_tracer {
    template<class ParamsT>
    class IFresnel
    {
    public:
        virtual Spectrum Evaluate(const ParamsT* params, Prec cos_theta_incoming) const = 0;
    };

    class FresnelNoOp final: public IFresnel<int>
    {
    public:
        virtual Spectrum Evaluate(const int* params, Prec cos_theta_incoming) const {return Spectrum::FromValue(1);};
    };
}