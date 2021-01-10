#include "LightIntegrator.h"
#include "core/IBSDF.h"
#include "core/ILight.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"

namespace integrators
{


    void LightIntegrator::Init(std::shared_ptr<core::IScene> scene){
        _scene = scene;
    }
    core::SpectrumPasses LightIntegrator::Integrate(const core::Ray& ray,const std::shared_ptr<core::ISampler>& sampler,core::MemoryArea& memory) const{
        auto surfaceInteraction = _scene->Intersect(ray);
        
        core::SpectrumPasses luminance;

        if(surfaceInteraction.has_value()){
            core::IBSDF* bsdf = surfaceInteraction->Material->ComputeBSDF(surfaceInteraction->Interaction, memory);
            for(const std::shared_ptr<core::ILight>& light : _scene->GetLights())
            {
                core::Vec3 interaction_point = surfaceInteraction->Interaction.Point + surfaceInteraction->Interaction.Normal* ERROR_THICCNESS;
                auto light_info = light->GetLightInformation(interaction_point);
                auto light_blocked = _scene->Intersect(core::Ray(interaction_point, -light_info.Direction));
                if(!light_blocked.has_value())
                {
                    core::SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                    core::SpectrumPasses bsdf_luminance = bsdf->Scattering(-ray.Direction, light_info.Direction);
                    luminance+= light_luminance*bsdf_luminance* std::abs(ray.Direction.dot(light_info.Direction));
                }
            }
            delete bsdf;
        }

        return luminance;
    }
    
}