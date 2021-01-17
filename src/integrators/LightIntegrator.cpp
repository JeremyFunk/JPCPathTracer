#include "LightIntegrator.h"
#include "core/Bsdf.h"
#include "core/ILight.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"

namespace integrators
{
    core::SpectrumPasses IntegrateLights(const core::Ray& ray,const core::SurfaceProperties& properties,const std::shared_ptr<core::IScene> scene,
    core::BsdfMemoryPtr& bsdf) 
    {
        core::SpectrumPasses luminance;
        for(const std::shared_ptr<core::ILight>& light : scene->GetLights())
        {
            core::Vec3 Prosurface_properties_point = properties.Interaction.Point + properties.Interaction.Normal* ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(Prosurface_properties_point);
            auto light_blocked = scene->Intersect(core::Ray(Prosurface_properties_point, -light_info.Direction));
            if(!light_blocked.has_value())
            {
                core::SpectrumPasses light_luminance = light->Illumination(Prosurface_properties_point, light_info);
                core::SpectrumPasses bsdf_luminance = core::ScatteringBsdf(bsdf,-ray.Direction, -light_info.Direction);
                luminance+= light_luminance*bsdf_luminance* std::abs(ray.Direction.dot(light_info.Direction));
            }
        }
        return luminance;
    }

    struct SampleCount
    {
        int X,Y;
    };

    void LightIntegrator::Init(std::shared_ptr<core::IScene> scene){
        _scene = scene;
    }

    std::unique_ptr<std::vector<core::Vec2>> LightIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return std::make_unique<std::vector<core::Vec2>>(sample_count.X*sample_count.Y);
    }
    
    void LightIntegrator::FillSamples(std::shared_ptr<core::ISampler> sampler, std::unique_ptr<std::vector<core::Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, 1, data->data());
    }
    core::Vec2* LightIntegrator::SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = sample_count.X*sample_idx_y+sample_idx_x;
        
        return samples+index;

    }
    core::SpectrumPasses LightIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,core::BsdfMemoryPtr bsdf_memory) const{
        auto surface_properties = _scene->Intersect(ray);
        
        core::SpectrumPasses luminance;

        if(surface_properties.has_value()){
            luminance+=surface_properties->Material->Illumination(surface_properties->Interaction, ray);
            surface_properties->Material->OverrideBSDF(bsdf_memory,surface_properties->Interaction);
            luminance+=IntegrateLights(ray,surface_properties.value(),_scene,bsdf_memory);
        }

        return luminance;
    }

    SampleCount LightIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = floor(sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }
    
}