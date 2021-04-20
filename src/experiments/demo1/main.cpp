#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include <memory>

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSampler());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4,1);
    
    jpctracer::JPCRenderer renderer(std::move(sampler),std::move(camera),std::move(integrator));
    renderer.ShouldMultiThread = false;
    
    renderer.MaterialLib.Register(
        "Default",
        [](jpctracer::MaterialSettings settings)
        {
            return jpctracer::ShaderBind(jpctracer::DebugBsdf, settings.GetColor("Color"));
        },
        {
            {"Color",jpctracer::FromRGB({1,0,1})}
        }
    );

    auto shader = renderer.MaterialLib.Get("Default");
    auto triangle = jpctracer::CreateTriangle({-5,5,-5}, {5,-5,-5}, {5,5,-5});

    triangle->MaterialSlots[0] = shader;

    auto sphere = jpctracer::CreateSphere({1,1,-2}, 0.5);
    sphere->MaterialSlots[0] = shader;

    //auto monkey = jpctracer::LoadMesh("/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/Susan.obj");

    auto cube = jpctracer::LoadMesh("/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/cube.obj");
    cube->transformation = jpctracer::RotScalTrans({0,0,-4}, 1, {0,0,0});
    cube->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    //renderer.Draw(sphere);
    renderer.Draw(cube);
    renderer.LightsLib.AddPointLight({0,-2,0}, jpctracer::FromRGB({10,10,10}));

    //Peer
    renderer.ShouldMultiThread = true;
    renderer.Acceleration = { jpctracer::raytracing::DynamicBVHType::NAIVE, jpctracer::raytracing::StaticBVHType::LBVH };

    //Chris
    renderer.Render(500, 300, "");

    return 0;
}