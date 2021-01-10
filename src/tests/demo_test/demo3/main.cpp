#include "core/Linalg.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BVHScene.h"
#include "integrators/LightIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "scene_setup.h"
#include <memory>


int main()
{
    int width = 1080;
    int height = 720;
    int sample_count = 10;
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