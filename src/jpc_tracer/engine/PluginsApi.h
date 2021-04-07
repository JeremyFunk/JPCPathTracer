#pragma once
#include"shadersystem/shadersystem.h"
#include "raytracing/Geometry.h"
#include <utility>

namespace jpctracer
{
    using IBsdfClosure = shadersys::IBsdfClosure;
    using NormalSpace = shadersys::NormalSpace;
    using BsdfNode = shadersys::BsdfNode;
    using ShaderContext = shadersys::ShaderContext;

    template<class func, class... Args>
    inline constexpr auto ShaderBind(const func& f, Args&&...args)
    {
        return shadersys::ShaderBind(f, std::forward<Args>(args)...);
    }


    template<int type,std::derived_from<IBsdfClosure> BsdfT, class... Args>
    inline constexpr BsdfNode* CreateBsdf(ShaderContext context, Args&&...args)
    {
        if(ContainsMaterialType(context.ray_type,type))
            return context.bsdf_stack->emplace<BsdfT,Args...>(std::forward<Args>(args)...);
        else
            return nullptr;
    }

    constexpr BsdfNode* MixBsdf(ShaderContext context, BsdfNode* first,BsdfNode* second,Prec mix_factor)
    {
        return context.bsdf_stack->MixNodes(first, second, mix_factor);
    }

    using TriangleGeometry = raytracing::TriangleGeometry;
    using TriangleShading = raytracing::TriangleShading;
    using TriangleMesh = raytracing::TriangleMesh;
    using SphereMesh = raytracing::SphereMesh;
    
}