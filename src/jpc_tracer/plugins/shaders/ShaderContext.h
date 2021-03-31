#pragma once
#include "bsdf/BsdfStack.h"
#include "cache/NormalSpace.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/plugins/shaders/bsdf/IBsdfClosure.h"

namespace jpctracer
{
    struct ShaderContext
    {
        const NormalSpace* normal_space;
        const MaterialType ray_type;
        BsdfStack* const bsdf_stack;
    };

    template<int type,std::derived_from<IBsdfClosure> BsdfT,
        class... Args>
    inline constexpr BsdfNode* CreateBsdf(ShaderContext context, Args&&...args)
    {
        if(ContainsMaterialType(context.ray_type,type))
            return context.bsdf_stack->emplace<BsdfT,Args...>(std::forward<Args>(args)...);
        else
            return nullptr;
    }

}