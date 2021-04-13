#pragma once
#include "ShaderContext.h"
#include "IBsdfClosure.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "DiscreteSampler.h"
#include "BsdfStack.h"
#include "ShaderFunction.h"
#include <utility>
namespace jpctracer::shadersys
{
    
    class RootShader
    {
    public:
        RootShader(LinearCombBsdfs&& bsdfs, const NormalSpace& space);

        RootShader(const RootShader &) = delete;
        RootShader(RootShader &&shader) = delete;

        std::pair<Spectrum, Prec> operator()(const Ray &incident_ray) const;

        std::pair<Spectrum, Prec> operator()(Ray *out_incident_ray,
                                            Vec2 random_point) const;

        std::pair<Spectrum, Prec> operator()(Ray *out_incident_ray,
                                            Vec3 random_point) const;
    
        ~RootShader();

    private:
        DiscreteSampler m_sampler;
        LinearCombBsdfs m_bsdfs;
        const NormalSpace& m_space;      

        template<class SampleT>
        std::pair<Spectrum,Prec> SampledEval(Ray* out_incident_ray, 
                                                SampleT random_point) const;
    };

    

    template<MaterialType type, ShaderFunction<BsdfNode*> FuncT>
    RootShader CreateShader(const FuncT& shader,BsdfStack* bsdf_stack,NormalSpace n_space)
    {
        ShaderContext context = {&n_space,type,bsdf_stack};
        BsdfNode* root_node = shader(context);
        LinearCombBsdfs&& bsdfs = bsdf_stack->pop_until(root_node);
        return {std::forward<LinearCombBsdfs>(bsdfs),std::move(n_space)};
    }
}