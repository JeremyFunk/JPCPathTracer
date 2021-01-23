#include "DebugIntegrator.h"
#include "core/Linalg.h"
#include <memory>
namespace jpc_tracer {

    struct SampleCount
    {
        int X,Y;
    };

    void DebugIntegrator::Init(std::shared_ptr<IScene> scene)
    {
        _scene = scene;
    }
    
    std::unique_ptr<std::vector<Vec2>> DebugIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<Vec2>>(sample_count.X*sample_count.Y);
    }
    
    void DebugIntegrator::FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, 1, data->data());
    }
    Vec2* DebugIntegrator::SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = sample_count.X*sample_idx_y+sample_idx_x;
        
        return samples+index;

    }
    

    SpectrumPasses DebugIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr memory) const
    {
        std::optional<SurfaceProperties> surfaceInteraction = _scene->Intersect(ray);
    
        SpectrumPasses luminance;
        if(surfaceInteraction.has_value()){
            
            Vec3 rgb = PixelEffect(surfaceInteraction.value(),ray,*samples,memory,_scene);
            luminance = SpectrumPasses(Spectrum::FromRGB(rgb));
        }
        return luminance;
    }
    
    SampleCount DebugIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = std::floor(std::sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
}