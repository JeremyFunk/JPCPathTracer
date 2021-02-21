#include "core/Linalg.h"
#include "core/SurfaceProperties.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BVHScene.h"
#include "scenes/LBVHScene.h"
#include "integrators/LightIntegrator.h"
#include "integrators/DebugIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "scene_setup.h"
#include <memory>
namespace jpc_tracer {

class TestIntegrator : public DebugIntegrator
{
public:
    virtual Vec3 PixelEffect(SurfaceProperties& properties, const Ray& ray, const Vec2& sample, BsdfMemoryPtr& memory,
        const Ref<IScene>& scene) const
    {
        return {0,0,0};
        //Vec3 normal = properties.Interaction.Normal;
        //Vec3 color = Vec3{0.5,0.5,0.5}+normal*0.5;
        //return color;

        //IBSDF* bsdf = properties.Material->ComputeBSDF(properties.Interaction, memory);
        
        for (const Ref<ILight>& light : scene->GetLights())
        {
            Vec3 interaction_point = properties.Interaction.Point + properties.Interaction.Normal * ERROR_THICCNESS;
            auto light_info = light->GetLightInformation(interaction_point);
            auto light_blocked = scene->Intersect(Ray(interaction_point, -light_info.Direction));
            if (!light_blocked.has_value())
            {
                SpectrumPasses light_luminance = light->Illumination(interaction_point, light_info);
                SpectrumPasses bsdf_luminance ;//= bsdf->Scattering(-ray.Direction, -light_info.Direction);
                return bsdf_luminance.GetCombined().ToRGB();
                //luminance += light_luminance * bsdf_luminance * std::abs(ray.Direction.dot(light_info.Direction));
            }
        }
        
    }
};


}

int main()
{
    using namespace jpc_tracer;
    int width = 1080;
    int height = 720;
    int sample_count = 5;
    auto sampler = MakeRef<StratifiedSampler>();
    auto camera = MakeRef<ProjectionCamera>(width,height,1);

    //Scene Setup
    auto shapeList = generate_shapes();
    auto lightList = generate_lights();

    auto scene = MakeRef<BVHScene>(shapeList, lightList);
    auto integrator = MakeRef<LightIntegrator>();
    integrator->Init(scene);
    auto filter = MakeRef<GaussianFilter>(0.5);
    auto film = MakeRef<BasicFilm>(width,height);
    auto renderer = MakeRef<BasicRenderer>(sample_count,true);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("demo3.png");
    return 0;
}