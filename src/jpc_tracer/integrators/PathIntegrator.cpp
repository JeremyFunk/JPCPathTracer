#include "PathIntegrator.h"
#include "LightIntegrator.h"
#include "core/Bsdf.h"
#include "core/Linalg.h"
#include "core/Ray.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include <iostream>
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
    
    SpectrumPasses PathIntegrator::Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const 
    {
        if(ray.Depth>=_max_depth)
            return Spectrum::FromValue(0);
        std::optional<SurfaceProperties> surface_properties = _scene->Intersect(ray);
    
        SpectrumPasses luminance;
        if(surface_properties.has_value()){
            luminance+=surface_properties->Material->Illumination(surface_properties->Interaction, ray);
            surface_properties->Material->OverrideBSDF( bsdf_memory,surface_properties->Interaction);
            luminance+=IntegrateLights(ray,surface_properties.value(),_scene,bsdf_memory);

            Vec3 interaction_point = surface_properties->Interaction.Point + surface_properties->Interaction.Normal* ERROR_THICCNESS;

            Vec3 incident_dir = SampleIncidentDirectionBsdf(bsdf_memory,-ray.Direction, *samples);
            samples++;
            Prec pdf = PdfBsdf(bsdf_memory,-ray.Direction, incident_dir);
            Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
            SpectrumPasses surface_color = ScatteringBsdf(bsdf_memory,-ray.Direction, incident_dir);

            if( (! surface_color.IsZero() ) && pdf > 0)
            {
                SpectrumPasses subray_radiance = Integrate(incident_ray,samples, std::move(bsdf_memory));
                luminance += subray_radiance/pdf * surface_color * std::abs(incident_dir.dot(surface_properties->Interaction.Normal));
            }

        }else {
            luminance = _sky_color;
        }
        return luminance;
    }

    
}