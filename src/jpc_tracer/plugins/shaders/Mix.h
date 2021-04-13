#pragma once

#include "jpc_tracer/engine/PluginsApi.h"
namespace jpctracer
{
    constexpr Prec MixPrec(Prec first,Prec second,Prec mix_factor)
    {
        return first*mix_factor+second*(1-mix_factor);
    }

    constexpr Vec3 MixVec3(Vec3 first,Vec3 second,Prec mix_factor)
    {
        return first*mix_factor+second*(1-mix_factor);
    }
    

    constexpr Spectrum MixSpec(Spectrum first,Spectrum second,Prec mix_factor)
    {
        return first*mix_factor+second*(1-mix_factor);
    }
    
    
    
}