#include "DebugBsdfIntegrator.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <memory>
namespace jpc_tracer {
    DebugBsdfIntegrator::DebugBsdfIntegrator(int max_depth) 
        : _max_depth(max_depth)
    {
        
    }


    struct SampleCount
    {
        int X,Y;
    };

    void DebugBsdfIntegrator::Init(std::shared_ptr<IScene> scene)
    {
        _scene = scene;
    }
    
    std::unique_ptr<std::vector<Vec2>> DebugBsdfIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<Vec2>>(sample_count.X*sample_count.Y*_max_depth);
    }
    
    void DebugBsdfIntegrator::FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, _max_depth, data->data());
    }
    Vec2* DebugBsdfIntegrator::SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = (sample_count.X*sample_idx_y+sample_idx_x)*_max_depth;
        
        return samples+index;

    }
    

    SpectrumPasses DebugBsdfIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const
    {
        
        std::optional<SurfaceProperties> surface_properties = _scene->Intersect(ray);
    
        if(surface_properties.has_value()){
            if(ray.Depth>=_max_depth-1){
                Vec3 rgb = PixelEffect(surface_properties.value(),ray,*samples,bsdf_memory,_scene);
                return Spectrum::FromRGB(rgb);
            }
            
            surface_properties->Material->OverrideBSDF( bsdf_memory,surface_properties->Interaction);

            Vec3 interaction_point = surface_properties->Interaction.Point + surface_properties->Interaction.Normal* ERROR_THICCNESS;

            Vec3 incident_dir = SampleIncidentDirectionBsdf(bsdf_memory,-ray.Direction, *samples);
            samples++;
            
            Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
            SpectrumPasses surface_color = ScatteringBsdf(bsdf_memory,-ray.Direction, incident_dir);

            

            return Integrate(incident_ray,samples, std::move(bsdf_memory));


        }
            
        return SpectrumPasses::FromValue(0);
    }
    
    SampleCount DebugBsdfIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = std::floor(std::sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
}