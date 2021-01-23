#include "BasicIntegrator.h"


namespace jpc_tracer
{


    void BasicIntegrator::Init(Ref<IScene> scene){
        _scene = scene;
    }
    
    Scope<std::vector<Vec2>> BasicIntegrator::SetupSamples(int max_sample_count) const 
    {
        return nullptr;
    }
    
    void BasicIntegrator::FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        
    }
    
    Vec2* BasicIntegrator::SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const 
    {
        return nullptr;
    }
    

    SpectrumPasses BasicIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr memory) const{
        auto spectrum = RGBSpectrum::FromRGB(Vec3(ray.Direction[0]*.5+.5, ray.Direction[1]*.5+.5, ray.Direction[2]*.5+.5));
        return SpectrumPasses(spectrum);
    }

}