#pragma once
#include "bsdf/BsdfPool.h"
#include "cache/NormalSpace.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/plugins/shaders/bsdf/IBsdfClosure.h"

namespace jpctracer
{
    struct ShaderContext
    {
        const NormalSpace* normal_space;
        const MaterialType ray_type;
        BsdfPool* const bsdf_pool;
    };

    template<MaterialType type,std::derived_from<IBsdfClosure> BsdfT,
        class... Args>
    inline constexpr BsdfNode* CreateBsdf(ShaderContext context, Args&&...args)
    {
        if(ContainsMaterialType(context.ray_type,type))
            return context.bsdf_pool->Create<BsdfT,Args...>(std::forward<Args>(args)...);
        else
            return nullptr;
    }

}