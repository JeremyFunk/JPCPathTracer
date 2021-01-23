#include "TestLightIntegrator.h"
#include "core/Linalg.h"
#include <iostream>



core::Vec3 TestLightIntegrator::PixelEffect(core::SurfaceProperties& properties,const core::Ray& ray,const core::Vec2& sample,core::BsdfMemoryPtr& memory, 
            const std::shared_ptr<core::IScene>& scene) const
{
        //core::Vec3 normal = properties.Interaction.Normal;
        //core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
        //return color;
        
        properties.Material->OverrideBSDF(memory, properties.Interaction);
        
        for (const std::shared_ptr<core::ILight>& light : scene->GetLights())
        {
            
            core::Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal * ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(interaction_point);
            auto intersection = scene->Intersect(core::Ray(interaction_point, -light_info.Direction));
            bool light_blocked = true;
            if(intersection.has_value())
            {
                core::Prec blocked_distance = (intersection->Interaction.Point-properties.Interaction.Point).norm();
                if(blocked_distance > light_info.Distance)
                    light_blocked = false;
            }
            if (!light_blocked)
            {
                return {1,1,1};
                core::SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                core::SpectrumPasses bsdf_luminance = core::ScatteringBsdf(memory,-ray.Direction, -light_info.Direction);
                core::Prec cos_theta = std::abs(properties.Interaction.Normal.dot(light_info.Direction));
                //return {cos_theta,cos_theta,cos_theta};

                std::cout<<"Interaction Point"<<std::endl;
                std::cout<<interaction_point<<std::endl;
                std::cout<<"Light Direction"<<std::endl;
                std::cout<<light_info.Direction<<std::endl;
                std::cout<<"Normal"<<std::endl;
                std::cout<<properties.Interaction.Normal<<std::endl;

                core::Vec3 normal = properties.Interaction.Normal;
                core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
                return color;

                //luminance += light_luminance * bsdf_luminance * std::abs(ray.Direction.dot(light_info.Direction));
            }
            
        }
        return {0,0,0};
}
