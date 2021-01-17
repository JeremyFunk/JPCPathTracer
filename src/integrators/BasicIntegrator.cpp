#include "BasicIntegrator.h"


namespace integrators
{


    void BasicIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }
    
    std::unique_ptr<std::vector<Vec2>> BasicIntegrator::SetupSamples(int max_sample_count) const 
    {
        return nullptr;
    }
    
    void BasicIntegrator::FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        
    }
    
    core::Vec2* BasicIntegrator::SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const 
    {
        return nullptr;
    }
    

    SpectrumPasses BasicIntegrator::Integrate(const Ray& ray,const core::Vec2* samples,BsdfMemoryPtr memory) const{
        auto spectrum = RGBSpectrum::FromRGB(Vec3(ray.Direction[0]*.5+.5, ray.Direction[1]*.5+.5, ray.Direction[2]*.5+.5));
        return SpectrumPasses(spectrum);
    }

}