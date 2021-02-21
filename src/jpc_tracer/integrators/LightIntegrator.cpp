#include "LightIntegrator.h"
#include "core/Bsdf.h"
#include "core/ILight.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceInteraction.h"
#include "integrators/Integration.h"

namespace jpc_tracer
{

    bool IsLightBlocked(const LightInformation& light_info, const SurfaceInteraction& interaction,const Ref<IScene> scene,const Vec3& interaction_point)
    {
        auto intersection = scene->Intersect(Ray(interaction_point, -light_info.Direction));
        bool light_blocked = false;
        if(intersection.has_value())
        {
            Prec blocked_distance = (intersection->Interaction.Point-interaction_point).norm();
            if(blocked_distance > light_info.Distance)
                light_blocked = false;
            else
                light_blocked = true; 
        }

        return light_blocked;
    }

    SpectrumPasses IntegrateLights(const Ray& ray,const Vec3& interaction_point, const SurfaceInteraction& interaction,const Ref<IScene> scene,
    const BsdfMemoryPtr& bsdf) 
    {
        SpectrumPasses luminance;
        for(const Ref<ILight>& light : scene->GetLights())
        {
            auto light_info = light->GetLightInformation(interaction_point);
            
            bool light_blocked = IsLightBlocked(light_info,interaction,scene,interaction_point);
            if(!light_blocked)
            {
                SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                SpectrumPasses bsdf_luminance = ScatteringBsdf(bsdf,-ray.Direction, -light_info.Direction);
                
                luminance+= light_luminance*bsdf_luminance* CosinusWeight(interaction.Normal, -light_info.Direction);
            }
        }
        return luminance;
    }

    struct SampleCount
    {
        int X,Y;
    };

    void LightIntegrator::Init(Ref<IScene> scene){
        _scene = scene;
    }

    Scope<std::vector<Vec2>> LightIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return MakeScope<std::vector<Vec2>>(sample_count.X*sample_count.Y);
    }
    
    void LightIntegrator::FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const 
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
            Vec3 interaction_point = OffsetInteractionPoint(surface_properties->Interaction);
            luminance+=IntegrateLights(ray,interaction_point, surface_properties->Interaction,_scene,bsdf_memory);
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