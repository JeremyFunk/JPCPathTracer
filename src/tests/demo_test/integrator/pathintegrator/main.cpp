#include "core/Linalg.h"
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


class TestIntegrator : public integrators::DebugIntegrator
{
public:
    virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties, const core::Ray& ray, std::shared_ptr<core::ISampler> sampler, core::MemoryArea& memory,
        const std::shared_ptr<core::IScene>& scene) const
    {
        core::Vec3 normal = properties.Interaction.Normal;
        core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
        return color;       
        
    }
};

int main()
{
    int width = 500;
    int height = 300;
    int sample_count = 1000;
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