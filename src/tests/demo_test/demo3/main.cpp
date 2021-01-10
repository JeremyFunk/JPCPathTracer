#include "core/Linalg.h"
#include "core/SurfaceProperties.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BVHScene.h"
#include "integrators/LightIntegrator.h"
#include "integrators/DebugIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "scene_setup.h"
#include <memory>


class TestIntegrator : public integrators::DebugIntegrator
{
public:
    virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties, const core::Ray& ray, std::shared_ptr<core::ISampler> sampler, core::MemoryArea& memory,
        const std::shared_ptr<core::IScene>& scene) const
    {
        //core::Vec3 normal = properties.Interaction.Normal;
        //core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
        //return color;

        core::IBSDF* bsdf = properties.Material->ComputeBSDF(properties.Interaction, memory);
        for (const std::shared_ptr<core::ILight>& light : scene->GetLights())
        {
            core::Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal * ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(interaction_point);
            auto light_blocked = scene->Intersect(core::Ray(interaction_point, -light_info.Direction));
            if (!light_blocked.has_value())
            {
                core::SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                core::SpectrumPasses bsdf_luminance = bsdf->Scattering(-ray.Direction, -light_info.Direction);
                return bsdf_luminance.GetCombined().ToRGB();
                //luminance += light_luminance * bsdf_luminance * std::abs(ray.Direction.dot(light_info.Direction));
            }
        }
        delete bsdf;
        
    }
};

int main()
{
    int width = 1080;
    int height = 720;
    int sample_count = 5;
    auto sampler = std::make_shared<samplers::StratifiedSampler>();
    auto camera = std::make_shared<cameras::ProjectionCamera>(width,height,1);

    //Scene Setup
    auto shapeList = generate_shapes();
    auto lightList = generate_lights();

    auto scene = std::make_shared<scenes::BVHScene>(shapeList, lightList);
    auto integrator = std::make_shared<integrators::LightIntegrator>();
    integrator->Init(scene);
    auto filter = std::make_shared<filters::GaussianFilter>(0.5);
    auto film = std::make_shared<films::BasicFilm>(width,height);
    auto renderer = std::make_shared<renderers::BasicRenderer>(sample_count,true);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("demo3.png");
}