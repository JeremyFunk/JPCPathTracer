#include"Base.h"
#include "SpectrumPasses.h"
#include "core/Spectrum.h"
namespace jpc_tracer
{
    CombinedPass CombinedPass::FromValue(const Prec& value) 
    {
        return CombinedPass(Spectrum::FromValue(value));
    }

    CombinedPass sqrt(const CombinedPass &c){
        CombinedPass ret = c;
        ret.Sqrt();
        return ret;
    }
    CombinedPass pow(const CombinedPass &c, Prec pow){
        CombinedPass ret = c;
        ret.Sqrt();
        return ret;
    }
    CombinedPass lerp(Prec t, CombinedPass &s1, CombinedPass &s2){
        CombinedPass ret = s1;
        ret.Lerp(t, s2);
        return ret;
    }

    CombinedPass clamp(Prec low, Prec high, const CombinedPass &c) {
        CombinedPass ret = c;
        ret.Clamp(low, high);
        return ret;
    }
}