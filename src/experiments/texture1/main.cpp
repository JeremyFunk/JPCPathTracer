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
    renderer.ShouldMultiThread = false;
    
    renderer.MaterialLib.Register(
        "Default",
        [](jpctracer::MaterialSettings settings)
        {
            return jpctracer::ShaderBind(jpctracer::DebugBsdf, settings.GetTexture("Color"));
        },
        {
            {"Color",""}
        }
    );

    auto shader = renderer.MaterialLib.Get("Default");
    //chris
    shader->SetTexture("Color","/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/color_grid.png");
    auto triangle = jpctracer::CreateTriangle({-1,1,-2}, {1,-1,-2}, {1,1,-2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0,0,0}, jpctracer::FromRGB({1,1,1}));

    //Chris
    renderer.Render(300, 300, "");

    return 0;
}