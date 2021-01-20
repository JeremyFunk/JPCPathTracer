#include "DebugBsdfIntegrator.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <memory>
namespace integrators {
    DebugBsdfIntegrator::DebugBsdfIntegrator(int max_depth) 
        : _max_depth(max_depth)
    {
        
    }


    struct SampleCount
    {
        int X,Y;
    };

    void DebugBsdfIntegrator::Init(std::shared_ptr<core::IScene> scene)
    {
        _scene = scene;
    }
    
    std::unique_ptr<std::vector<core::Vec2>> DebugBsdfIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<core::Vec2>>(sample_count.X*sample_count.Y*_max_depth);
    }
    
    void DebugBsdfIntegrator::FillSamples(std::shared_ptr<core::ISampler> sampler, std::unique_ptr<std::vector<core::Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, _max_depth, data->data());
    }
    core::Vec2* DebugBsdfIntegrator::SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = (sample_count.X*sample_idx_y+sample_idx_x)*_max_depth;
        
        return samples+index;

    }
    

    core::SpectrumPasses DebugBsdfIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,BsdfMemoryPtr bsdf_memory) const
    {
        
        std::optional<core::SurfaceProperties> surface_properties = _scene->Intersect(ray);
    
        if(surface_properties.has_value()){
            if(ray.Depth>=_max_depth-1){
                core::Vec3 rgb = PixelEffect(surface_properties.value(),ray,*samples,bsdf_memory,_scene);
                return Spectrum::FromRGB(rgb);
            }
            
            surface_properties->Material->OverrideBSDF( bsdf_memory,surface_properties->Interaction);

            core::Vec3 interaction_point = surface_properties->Interaction.Point + surface_properties->Interaction.Normal* ERROR_THICCNESS;

            core::Vec3 incident_dir = core::SampleIncidentDirectionBsdf(bsdf_memory,-ray.Direction, *samples);
            samples++;
            
            core::Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
            core::SpectrumPasses surface_color = core::ScatteringBsdf(bsdf_memory,-ray.Direction, incident_dir);

            

            return Integrate(incident_ray,samples, std::move(bsdf_memory));


        }
            
        return SpectrumPasses::FromValue(0);
    }
    
    SampleCount DebugBsdfIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = floor(sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
}