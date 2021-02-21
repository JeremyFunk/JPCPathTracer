#include "PathIntegrator.h"
#include "LightIntegrator.h"
#include "core/Bsdf.h"
#include "core/Linalg.h"
#include "core/Logger.h"
#include "core/Ray.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceInteraction.h"
#include <iostream>
#include "core/MonteCarlo.h"
#include "integrators/Integration.h"
namespace jpc_tracer
{

    struct SampleCount
    {
        int X,Y;
    };

    SampleCount PathIntegrator::GetSampleCount(int max_sample_count) const
    {
        int sample_count_x = std::floor(std::sqrt(max_sample_count));
        int sample_count_y = sample_count_x;
        return {sample_count_x,sample_count_y};
    }

    PathIntegrator::PathIntegrator(SpectrumPasses sky_color, int max_depth)
        : _sky_color(sky_color), _max_depth(max_depth)
    {
        
    }

    void PathIntegrator::Init(Ref<IScene> scene) 
    {
        _scene =scene;
    }
    
    Scope<std::vector<Vec2>> PathIntegrator::SetupSamples(int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        return MakeScope<std::vector<Vec2>>(sample_count.X*sample_count.Y*_max_depth);
    }
    
    void PathIntegrator::FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const 
    {
        SampleCount sample_count = GetSampleCount(max_sample_count);
        sampler->Get2DSampleArray(sample_count.Y, sample_count.X, _max_depth, data->data());
    }
    Vec2* PathIntegrator::SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const 
    {


        SampleCount sample_count = GetSampleCount(max_sample_count);
        
        int sample_idx_x = sampling_idx % sample_count.X;
        int sample_idx_y = (sampling_idx / sample_count.X ) % sample_count.Y;
        int index = (sample_count.X*sample_idx_y+sample_idx_x)*_max_depth;
        
        return samples+index;

    }
    
    std::tuple<SpectrumPasses,Prec> PathIntegrator::IncidentLuminance(const Ray& ray,const Vec2* samples, const SurfaceInteraction& interaction,
    BsdfMemoryPtr bsdf_memory, Vec3 interaction_point) const
    {
        SpectrumPasses luminance;
        Vec3 incident_dir = SampleIncidentDirectionBsdf(bsdf_memory,-ray.Direction, *samples);
        samples++;
        Prec pdf = PdfBsdf(bsdf_memory,-ray.Direction, incident_dir);
        Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
        SpectrumPasses surface_color = ScatteringBsdf(bsdf_memory,-ray.Direction, incident_dir);

        if( (! surface_color.IsZero() ) && pdf > 0)
        {
            SpectrumPasses subray_radiance = Integrate(incident_ray,samples, bsdf_memory);
            luminance = subray_radiance * surface_color * CosinusWeight(incident_dir, interaction.Normal);
        }
        return {luminance,pdf};

    }

    SpectrumPasses PathIntegrator::SurfaceLuminance(const Ray& ray,const Vec2* samples, const SurfaceProperties& surface_properties,BsdfMemoryPtr bsdf_memory) const
    {
        
        
        const SurfaceInteraction& interaction = surface_properties.Interaction;

        SpectrumPasses luminance=surface_properties.Material->Illumination(interaction, ray);
        surface_properties.Material->OverrideBSDF( bsdf_memory,interaction);

        Vec3 interaction_point = OffsetInteractionPoint(interaction);

        auto light_lum = IntegrateLights(ray,interaction_point,interaction,_scene,bsdf_memory);
        auto[incident_lum, incident_pdf] = IncidentLuminance(ray, samples, interaction, bsdf_memory, interaction_point);
        
        
        auto temp = ImportanceSampling<SpectrumPasses,2>({light_lum,incident_lum},{1.0,incident_pdf});

        auto rbg = temp.GetCombined().ToRGB();
        auto rbg_lum = luminance.GetCombined().ToRGB();
        if(rbg[0]<0 || rbg[1]<0 || rbg[2]<0) JPC_LOG_ERROR("SubColor <0");


        if(incident_pdf <0.0001) 
        {
            //JPC_LOG_ERROR("Low PDf");
            temp = light_lum;  
        };

        luminance += temp;
        return luminance;
        
    }

    SpectrumPasses PathIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const 
    {
        if(ray.Depth>=_max_depth)
            return Spectrum::FromValue(0);
        std::optional<SurfaceProperties> surface_properties = _scene->Intersect(ray);
    
        SpectrumPasses luminance;
        if(surface_properties.has_value()){
            luminance=SurfaceLuminance(ray,samples,surface_properties.value(),bsdf_memory);

        }else {
            luminance = _sky_color;
        }
        return luminance;
    }

 
}