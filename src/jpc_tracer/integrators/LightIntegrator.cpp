#include "LightIntegrator.h"
#include "core/Bsdf.h"
#include "core/ILight.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"

namespace jpc_tracer
{
    SpectrumPasses IntegrateLights(const Ray& ray,const SurfaceProperties& properties,const std::shared_ptr<IScene> scene,
    BsdfMemoryPtr& bsdf) 
    {
        SpectrumPasses luminance;
        for(const std::shared_ptr<ILight>& light : scene->GetLights())
        {
            Vec3 Prosurface_properties_point = properties.Interaction.Point + properties.Interaction.Normal*ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(Prosurface_properties_point);
            auto intersection = scene->Intersect(Ray(Prosurface_properties_point, -light_info.Direction));
            bool light_blocked = false;
            if(intersection.has_value())
            {
                Prec blocked_distance = (intersection->Interaction.Point-properties.Interaction.Point).norm();
                if(blocked_distance > light_info.Distance)
                    light_blocked = false;
                else
                    light_blocked = true;
                
                
            }
            
            if(!light_blocked)
            {
                SpectrumPasses light_luminance = light->Illumination(Prosurface_properties_point, light_info);
                SpectrumPasses bsdf_luminance = ScatteringBsdf(bsdf,-ray.Direction, -light_info.Direction);
                luminance+= light_luminance*bsdf_luminance* std::abs(properties.Interaction.Normal.dot(-light_info.Direction));
            }
        }
        return luminance;
    }

    struct SampleCount
    {
        int X,Y;
    };

    void LightIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }

    std::unique_ptr<std::vector<Vec2>> LightIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<Vec2>>(sample_count.X*sample_count.Y);
    }
    
    void LightIntegrator::FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, 1, data->data());
    }
    Vec2* LightIntegrator::SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = sample_count.X*sample_idx_y+sample_idx_x;
        
        return samples+index;

    }
    SpectrumPasses LightIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const{
        auto surface_properties = _scene->Intersect(ray);
        
        SpectrumPasses luminance;

        if(surface_properties.has_value()){
            luminance+=surface_properties->Material->Illumination(surface_properties->Interaction, ray);
            surface_properties->Material->OverrideBSDF(bsdf_memory,surface_properties->Interaction);
            luminance+=IntegrateLights(ray,surface_properties.value(),_scene,bsdf_memory);
        }

        return luminance;
    }

    SampleCount LightIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = std::floor(std::sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
    
}