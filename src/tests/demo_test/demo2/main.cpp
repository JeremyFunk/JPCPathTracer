#include "core/Linalg.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BasicScene.h"
#include "integrators/BasicIntegrator.h"
#include "integrators/PathIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include <memory>


int main()
{
    int width = 480;
    int height = 360;
    auto sampler = std::make_shared<samplers::StratifiedSampler>();
    auto camera = std::make_shared<cameras::ProjectionCamera>(width,height,1);
    auto scene = std::make_shared<scenes::BasicScene>();
    auto integrator = std::make_shared<integrators::PathIntegrator>();
    integrator->Init(scene);
    auto filter = std::make_shared<filters::GaussianFilter>(0.5);
    auto film = std::make_shared<films::BasicFilm>(width,height);
    auto renderer = std::make_shared<renderers::BasicRenderer>();
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("demo2.png");
}