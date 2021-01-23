#include "core/Bsdf.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceProperties.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BVHScene.h"
#include "scenes/BasicScene.h"
#include "integrators/PathIntegrator.h"
#include "integrators/DebugBsdfIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "scene_setup.h"
#include <iostream>
#include <memory>
#include "TestLightIntegrator.h"
#include "scenes/BasicScene.h"


class IncidentDirIntegrator : public integrators::DebugBsdfIntegrator
{
public:
    IncidentDirIntegrator(int max_depth) : integrators::DebugBsdfIntegrator(max_depth) {}
    virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties, const core::Ray& ray, const core::Vec2& sample, core::BsdfMemoryPtr& memory,
        const std::shared_ptr<core::IScene>& scene) const
    {
        
        properties.Material->OverrideBSDF(memory,properties.Interaction);

        core::Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal* ERROR_THICCNESS;

        core::Vec3 incident_dir = core::SampleIncidentDirectionBsdf(memory,-ray.Direction, sample);
        core::Prec pdf = core::PdfBsdf(memory,-ray.Direction, incident_dir);
        core::Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
        core::SpectrumPasses surface_color = core::ScatteringBsdf(memory,-ray.Direction, incident_dir);


        core::Vec3 normal = properties.Interaction.Normal;//incident_ray.Direction;//ray.Direction - 2*ray.Direction.dot(properties.Interaction.Normal)*properties.Interaction.Normal;
        core::Vec3 color = core::Vec3{0.5,0.5,0.5}+normal*0.5;
        
        //return color;
        //return surface_color.GetCombined().ToRGB();
        return {pdf,pdf,pdf};
        //return {1,0,0};
        
    }
};

int main()
{
    int width = 500;
    int height = 300;
    int sample_count = 2;
    auto sampler = std::make_shared<samplers::StratifiedSampler>();
    auto camera = std::make_shared<cameras::ProjectionCamera>(width,height,1);

    //Scene Setup
    auto shapeList = generate_shapes();
    auto lightList = generate_lights();

    auto scene = std::make_shared<scenes::BVHScene>(shapeList, lightList);
    //auto scene = std::make_shared<scenes::BasicScene>(shapeList, lightList);
    auto integrator = std::make_shared<integrators::PathIntegrator>(core::Spectrum::FromRGB({0,0,0}),2);
    //auto integrator = std::make_shared<IncidentDirIntegrator>(1);
    //auto integrator = std::make_shared<TestLightIntegrator>();
    integrator->Init(scene);
    auto filter = std::make_shared<filters::GaussianFilter>(0.5);
    auto film = std::make_shared<films::BasicFilm>(width,height);
    auto renderer = std::make_shared<renderers::BasicRenderer>(sample_count,true);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("pathintegrator.png");
}