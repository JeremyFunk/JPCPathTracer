#include "DebugIntegrator.h"
#include "core/Linalg.h"
#include <memory>
namespace integrators {

    struct SampleCount
    {
        int X,Y;
    };

    void DebugIntegrator::Init(std::shared_ptr<core::IScene> scene)
    {
        _scene = scene;
    }
    
    std::unique_ptr<std::vector<core::Vec2>> DebugIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<core::Vec2>>(sample_count.X*sample_count.Y);
    }
    
    void DebugIntegrator::FillSamples(std::shared_ptr<core::ISampler> sampler, std::unique_ptr<std::vector<core::Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, 1, data->data());
    }
    core::Vec2* DebugIntegrator::SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = sample_count.X*sample_idx_y+sample_idx_x;
        
        return samples+index;

    }
    

    core::SpectrumPasses DebugIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,BsdfMemoryPtr memory) const
    {
        std::optional<core::SurfaceProperties> surfaceInteraction = _scene->Intersect(ray);
    
        core::SpectrumPasses luminance;
        if(surfaceInteraction.has_value()){
            
            core::Vec3 rgb = PixelEffect(surfaceInteraction.value(),ray,*samples,memory,_scene);
            luminance = core::SpectrumPasses(core::Spectrum::FromRGB(rgb));
        }
        return luminance;
    }
    
    SampleCount DebugIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = floor(sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
}