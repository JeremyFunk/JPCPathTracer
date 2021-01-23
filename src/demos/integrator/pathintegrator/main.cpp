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
namespace jpc_tracer {

class IncidentDirIntegrator : public DebugBsdfIntegrator
{
public:
    IncidentDirIntegrator(int max_depth) : DebugBsdfIntegrator(max_depth) {}
    virtual Vec3 PixelEffect(SurfaceProperties& properties, const Ray& ray, const Vec2& sample, BsdfMemoryPtr& memory,
        const Ref<IScene>& scene) const
    {
        
        properties.Material->OverrideBSDF(memory,properties.Interaction);

        Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal* ERROR_THICCNESS;

        Vec3 incident_dir = SampleIncidentDirectionBsdf(memory,-ray.Direction, sample);
        Prec pdf = PdfBsdf(memory,-ray.Direction, incident_dir);
        Ray incident_ray(interaction_point,incident_dir,ray.Depth+1);
        SpectrumPasses surface_color = ScatteringBsdf(memory,-ray.Direction, incident_dir);


        Vec3 normal = properties.Interaction.Normal;//incident_ray.Direction;//ray.Direction - 2*ray.Direction.dot(properties.Interaction.Normal)*properties.Interaction.Normal;
        Vec3 color = Vec3{0.5,0.5,0.5}+normal*0.5;
        
        //return color;
        //return surface_color.GetCombined().ToRGB();
        return {pdf,pdf,pdf};
        //return {1,0,0};
        
    }
};


}

int main()
{
    using namespace jpc_tracer;
    int width = 500;
    int height = 300;
    int sample_count = 64;
    auto sampler = MakeRef<StratifiedSampler>();
    auto camera = MakeRef<ProjectionCamera>(width,height,1);

    //Scene Setup
    auto shapeList = generate_shapes();
    auto lightList = generate_lights();
    

    auto scene = MakeRef<BVHScene>(shapeList, lightList);
    //auto scene = MakeRef<BasicScene>(shapeList, lightList);
    auto integrator = MakeRef<PathIntegrator>(Spectrum::FromRGB({0,0,0}),2);
    //auto integrator = MakeRef<IncidentDirIntegrator>(1);
    //auto integrator = MakeRef<TestLightIntegrator>();
    integrator->Init(scene);
    auto filter = MakeRef<GaussianFilter>(0.5);
    auto film = MakeRef<BasicFilm>(width,height);
    auto renderer = MakeRef<BasicRenderer>(sample_count,true);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("pathintegrator.png");
}