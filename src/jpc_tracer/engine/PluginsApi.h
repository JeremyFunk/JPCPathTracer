#pragma once
#include "jpc_tracer/engine/shadersystem/HitPoint.h"
#include "jpc_tracer/engine/renderer/Tracer.h"
#include "jpc_tracer/engine/shadersystem/BsdfNode.h"
#include "jpc_tracer/engine/films/Film.h"
#include "jpc_tracer/engine/renderer/RenderInterfaces.h"
#include"shadersystem/shadersystem.h"

#include "raytracing/Geometry.h"
#include <utility>

namespace jpctracer
{
    
    using IBsdfClosure = shadersys::IBsdfClosure;
    using NormalSpace = shadersys::NormalSpace;
    using BsdfNode = shadersys::BsdfNode;
    using ShaderContext = shadersys::ShaderContext;
    using ShaderResult = shadersys::ShaderResult;
    using ShaderResults = shadersys::ShaderResults;

    template<class func, class... Args>
    inline constexpr auto ShaderBind(func&& f, Args&&...args)
    {
        return shadersys::ShaderBind(f, std::forward<Args>(args)...);
    }


    template<int type,std::derived_from<IBsdfClosure> BsdfT, class... Args>
    inline constexpr BsdfNode CreateBsdf(ShaderContext& ctx, Args...args)
    {
        if(ContainsMaterialType(type,ctx.ray_type))
            return shadersys::__CreateBsdf<BsdfT>(ctx,std::forward<Args>(args)...);
        
        return {-1,true};
    }

    inline BsdfNode MixBsdf(ShaderContext& ctx, BsdfNode first,BsdfNode second,Prec mix_factor)
    {
        return shadersys::__MixBsdf(ctx,first,second,mix_factor);
    }

    using IRayBehavior = renderer::IRayBehavior;
    using Tracer = renderer::Tracer;
    using HitPoint = shadersys::HitPoint;
    
    using TriangleGeometry = raytracing::TriangleGeometry;
    using TriangleShading = raytracing::TriangleShading;
    using TriangleMesh = raytracing::TriangleMesh;
    using SphereMesh = raytracing::SphereMesh;

    using Film = film::Film;

    using ICamera = renderer::ICamera;
    using ISampler = renderer::ISampler;
    using IIntegrator = renderer::IIntegrator;
    
}