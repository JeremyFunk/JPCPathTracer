#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceProperties.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BVHScene.h"
#include "integrators/PathIntegrator.h"
#include "integrators/DebugIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "scene_setup.h"
#include <memory>


class IncidentDirIntegrator : public integrators::DebugIntegrator
{
public:
    virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties, const core::Ray& ray, const core::Vec2& sample, core::MemoryArea& memory,
        const std::shared_ptr<core::IScene>& scene) const
    {
        
        core::IBSDF* bsdf = properties.Material->ComputeBSDF(properties.Interaction, memory);

        core::Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal* ERROR_THICCNESS;

        core::Vec3 incident_dir = bsdf->SampleIncidentDirection(-ray.Direction, sample);
        core::Prec pdf = bsdf->Pdf(-ray.Direction, incident_dir);
        core::Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
        core::SpectrumPasses surface_color = bsdf->Scattering(-ray.Direction, incident_dir);
        
        delete bsdf;

        core::Vec3 normal = incident_dir;
        core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
        return color;  
        
    }
};

int main()
{
    int width = 500;
    int height = 300;
    int sample_count = 625;
    auto sampler = std::make_shared<samplers::StratifiedSampler>();
    auto camera = std::make_shared<cameras::ProjectionCamera>(width,height,1);

    //Scene Setup
    auto shapeList = generate_shapes();
    auto lightList = generate_lights();

    auto scene = std::make_shared<scenes::BVHScene>(shapeList, lightList);
    auto integrator = std::make_shared<integrators::PathIntegrator>();
    integrator->Init(scene);
    auto filter = std::make_shared<filters::GaussianFilter>(0.5);
    auto film = std::make_shared<films::BasicFilm>(width,height);
    auto renderer = std::make_shared<renderers::BasicRenderer>(sample_count,true);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("pathintegrator.png");
}