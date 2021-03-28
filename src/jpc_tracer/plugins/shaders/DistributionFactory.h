#pragma once
#include "bsdf/RootShader.h"
#include "jpc_tracer/core/MaterialType.h"
#include "ShaderFunction.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/plugins/shaders/ShaderContext.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfPool.h"
#include "jpc_tracer/plugins/shaders/cache/NormalSpace.h"
#include "jpc_tracer/plugins/shaders/lights/Lights.h"
#include "lights/LightsDistribution.h"

namespace jpctracer
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
        BsdfPool m_shader_pool;

    };


    template<MaterialType type,ShaderFunction<BsdfNode*> ShaderT>
    RootShader DistributionFactory::Create(const ShaderT& shader, const Ray& scattering_ray,const SurfaceInteraction& interaction)
    {
        NormalSpace n_space = CreateNormalSpace(scattering_ray, interaction);
        return CreateShader<type>(shader,&m_shader_pool,std::move(n_space));
    }
}