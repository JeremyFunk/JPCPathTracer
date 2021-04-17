#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
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
    auto triangle = jpctracer::CreateTriangle({-1,1,-2}, {1,-1,-2}, {1,1,-2});

    triangle->MaterialSlots[0] = shader;

    auto sphere = jpctracer::CreateSphere({0,0,-2}, 0.5);
    sphere->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.Draw(sphere);
    renderer.LightsLib.AddPointLight({0,0,0}, jpctracer::FromRGB({1,1,1}));

    //Peer
    renderer.ShouldMultiThread = true;
    renderer.Acceleration = { jpctracer::raytracing::DynamicBVHType::NAIVE, jpctracer::raytracing::StaticBVHType::LBVH };

    //Chris
    renderer.Render(300, 300, "");

    return 0;
}