#include "RootShader.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/cache/NormalSpace.h"



namespace jpctracer
{
    RootShader::RootShader(LinearCombBsdfs bsdfs, BsdfFactorySaver saver, const NormalSpace& space) 
        : m_space(space),m_sampler(bsdfs.weights, bsdfs.count),m_bsdfs(bsdfs),m_factory_saver(saver)
    {
        
    }
    
    
    std::pair<Spectrum,Prec> RootShader::operator()(const Ray& incident_ray) const
    {
        Ray incident_ray_ns = WorldToNormal(incident_ray,m_space);
        Spectrum luminance = FromValue(0);
        Prec pdf = 0;
        for(int i = 0; i<m_bsdfs.count;i++)
        {
            auto[bsdf_lum,bsdf_pdf] = m_bsdfs.closures[i]->operator()(m_space,incident_ray_ns);
            Prec weight = m_bsdfs.weights[i];   
            luminance+=bsdf_lum*weight;
            pdf+=bsdf_pdf*weight;
        }
        return {luminance,pdf};


    }

    template<class SampleT>
    std::pair<Spectrum,Prec> RootShader::SampledEval(Ray* out_incident_ray, 
                                                SampleT random_point) const
    {
        auto[sampled_idx,new_rand_p] = m_sampler.Sample(random_point[0]);
        random_point[0] = new_rand_p;
        auto[luminance,pdf] = m_bsdfs.closures[sampled_idx]->operator()(m_space,out_incident_ray,random_point);
        for(int i = 0; i<m_bsdfs.count;i++)
        {
            if(i!=sampled_idx) continue;
             auto[bsdf_lum,bsdf_pdf] = m_bsdfs.closures[i]->operator()(m_space,*out_incident_ray);
            Prec weight = m_bsdfs.weights[i];   
            luminance+=bsdf_lum*weight;
            pdf+=bsdf_pdf*weight;
        }
        *out_incident_ray = NormalToWorld(*out_incident_ray,m_space);
        return {luminance,pdf};
    }
    
    std::pair<Spectrum,Prec> RootShader::operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
    {
        return SampledEval(out_incident_ray,random_point);
    }
    
    std::pair<Spectrum,Prec> RootShader::operator()(Ray* out_incident_ray, 
                                                Vec3 random_point) const
    {
        return SampledEval(out_incident_ray,random_point);
    }
    
    RootShader::~RootShader() 
    {
        m_factory_saver.Restore();
    }
}