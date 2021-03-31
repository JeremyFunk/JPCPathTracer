#pragma once
#include "BsdfStack.h"
#include "NormalSpace.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/engine/shadersystem/IBsdfClosure.h"

namespace jpctracer::shadersys
{
    struct ShaderContext
    {
        const NormalSpace* normal_space;
        const MaterialType ray_type;
        BsdfStack* const bsdf_stack;
    };

    

}