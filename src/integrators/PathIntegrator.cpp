#include "PathIntegrator.h"
#include "LightIntegrator.h"
#include "core/Linalg.h"
#include "core/Ray.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
namespace integrators
{
    PathIntegrator::PathIntegrator(core::SpectrumPasses sky_color, int max_depth)
        : _sky_color(sky_color), _max_depth(max_depth)
    {
        
    }

    void PathIntegrator::Init(std::shared_ptr<core::IScene> scene) 
    {
        _scene =scene;
    }
    
    size_t PathIntegrator::Get2DSampleCount() 
    {
        return _max_depth;
    }
    
    core::SpectrumPasses PathIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,core::MemoryArea& memory) const 
    {
        if(ray.Depth>=_max_depth)
            return core::Spectrum::FromValue(0);
        std::optional<core::SurfaceProperties> surfaceInteraction = _scene->Intersect(ray);
    
        core::SpectrumPasses luminance;
        if(surfaceInteraction.has_value()){
            luminance+=surfaceInteraction->Material->Illumination(surfaceInteraction->Interaction, ray);
            core::IBSDF* bsdf = surfaceInteraction->Material->ComputeBSDF(surfaceInteraction->Interaction, memory);
            luminance+=IntegrateLights(ray,surfaceInteraction.value(),_scene,bsdf);

            core::Vec3 interaction_point = surfaceInteraction->Interaction.Point + surfaceInteraction->Interaction.Normal* ERROR_THICCNESS;

            core::Vec3 incident_dir = bsdf->SampleIncidentDirection(-ray.Direction, *samples);
            samples++;
            core::Prec pdf = bsdf->Pdf(-ray.Direction, incident_dir);
            core::Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
            core::SpectrumPasses surface_color = bsdf->Scattering(-ray.Direction, incident_dir);
            
            delete bsdf;
            if( (! surface_color.IsZero() ) && pdf > 0)
            {
                core::SpectrumPasses subray_radiance = Integrate(incident_ray,samples, memory);
                luminance += subray_radiance/pdf * surface_color * std::abs(ray.Direction.dot(incident_dir));;
            }

        }else {
            luminance = _sky_color;
        }
        return luminance;
    }
}