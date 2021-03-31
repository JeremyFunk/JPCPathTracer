#pragma once
#include "RootShader.h"
#include "jpc_tracer/core/MaterialType.h"
#include "ShaderFunction.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/shadersystem/ShaderContext.h"
#include "jpc_tracer/engine/shadersystem/BsdfStack.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "LightsDistribution.h"

namespace jpctracer::shadersys
{
    class DistributionFactory
    {
    public:
        DistributionFactory Clone();

        template<MaterialType type,ShaderFunction<BsdfNode*> ShaderT>
        RootShader Create(const ShaderT& shader, const Ray& scattering_ray,const SurfaceInteraction& interaction);
        
        template<MaterialType type>
        LightsDistribution Create(const Lights& lights, const SurfaceInteraction& interaction);
    
    private:
        BsdfStack m_shader_pool;

    };


    template<MaterialType type,ShaderFunction<BsdfNode*> ShaderT>
    RootShader DistributionFactory::Create(const ShaderT& shader, const Ray& scattering_ray,const SurfaceInteraction& interaction)
    {
        NormalSpace n_space = CreateNormalSpace(scattering_ray, interaction);
        return CreateShader<type>(shader,&m_shader_pool,std::move(n_space));
    }
}