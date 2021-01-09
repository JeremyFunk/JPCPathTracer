#pragma once
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
namespace bsdfs {
    class IFresnel
    {
    public:
        virtual core::Spectrum Evaluate( core::Prec cos_theta_incoming) const = 0;
    };

    class FresnelNoOp : public IFresnel
    {
    public:
        virtual core::Spectrum Evaluate( core::Prec cos_theta_incoming) const {return core::Spectrum::FromValue(1);};
    };
}