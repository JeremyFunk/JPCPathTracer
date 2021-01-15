#include "LightIntegrator.h"
#include "core/IBSDF.h"
#include "core/ILight.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"

namespace integrators
{
    core::SpectrumPasses IntegrateLights(const core::Ray& ray,const core::SurfaceProperties& properties,const std::shared_ptr<core::IScene> scene,core::IBSDF* bsdf) 
    {
        core::SpectrumPasses luminance;
        for(const std::shared_ptr<core::ILight>& light : scene->GetLights())
        {
            core::Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal* ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(interaction_point);
            auto light_blocked = scene->Intersect(core::Ray(interaction_point, -light_info.Direction));
            if(!light_blocked.has_value())
            {
                core::SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                core::SpectrumPasses bsdf_luminance = bsdf->Scattering(-ray.Direction, -light_info.Direction);
                luminance+= light_luminance*bsdf_luminance* std::abs(ray.Direction.dot(light_info.Direction));
            }
        }
        return luminance;
    }



    void LightIntegrator::Init(std::shared_ptr<core::IScene> scene){
        _scene = scene;
    }
    
    size_t LightIntegrator::Get2DSampleCount() 
    {
        return 1;
    }
    core::SpectrumPasses LightIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,core::MemoryArea& memory) const{
        auto surfaceInteraction = _scene->Intersect(ray);
        
        core::SpectrumPasses luminance;

        if(surfaceInteraction.has_value()){
            luminance+=surfaceInteraction->Material->Illumination(surfaceInteraction->Interaction, ray);
            core::IBSDF* bsdf = surfaceInteraction->Material->ComputeBSDF(surfaceInteraction->Interaction, memory);
            luminance+=IntegrateLights(ray,surfaceInteraction.value(),_scene,bsdf);
            delete bsdf;
        }

        return luminance;
    }
    
}