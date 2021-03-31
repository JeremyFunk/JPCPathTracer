#pragma once

#include "../ShaderContext.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfStack.h"
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
    
    constexpr BsdfNode* MixBsdf(ShaderContext context, BsdfNode* first,BsdfNode* second,Prec mix_factor)
    {
        return context.bsdf_stack->MixNodes(first, second, mix_factor);
    }
    
}