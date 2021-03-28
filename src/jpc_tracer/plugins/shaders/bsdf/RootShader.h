#pragma once
#include "../ShaderContext.h"
#include "IBsdfClosure.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/plugins/shaders/cache/NormalSpace.h"
#include "../sampling/DiscreteSampler.h"
#include "BsdfPool.h"
#include "../ShaderFunction.h"
namespace jpctracer
{
    
    class RootShader
    {
    public:

        RootShader(LinearCombBsdfs bsdfs, BsdfFactorySaver saver, const NormalSpace& space);

        RootShader(const RootShader&) = delete;
        RootShader(RootShader&& shader) = default;

        std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec2 random_point) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec3 random_point) const;

        ~RootShader();

    private:
        detail::DiscreteSampler m_sampler;
        LinearCombBsdfs m_bsdfs;
        const NormalSpace& m_space;      
        BsdfFactorySaver m_factory_saver; 

        template<class SampleT>
        std::pair<Spectrum,Prec> SampledEval(Ray* out_incident_ray, 
                                                SampleT random_point) const;
    };

    

    template<MaterialType type, ShaderFunction<BsdfNode*> FuncT>
    RootShader CreateShader(const FuncT& shader,BsdfPool* bsdf_pool,NormalSpace n_space)
    {
        BsdfFactorySaver save_point(bsdf_pool);
        BsdfMemoryState state = bsdf_pool->BsdfsState();
        ShaderContext context = {&n_space,type,bsdf_pool};
        BsdfNode* root_node = shader(context);
        context.bsdf_pool->ApplyWeights(root_node);
        context.bsdf_pool->ClearBsdfNodes();
        LinearCombBsdfs bsdfs = bsdf_pool->GetBsdfs(state);
        return RootShader{bsdfs,save_point,std::move(n_space)};
    }
}