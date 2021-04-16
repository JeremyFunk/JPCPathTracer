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
    using sampler_t = decltype(jpctracer::sampler::StratifiedSampler2());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSampler2());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4,4);
    
    jpctracer::JPCRenderer renderer(std::move(sampler),std::move(camera),std::move(integrator));
    renderer.TileSize = 64;
    renderer.ShouldMultiThread = false;
    
    renderer.MaterialLib.Register(
        "Default",
        [](jpctracer::MaterialSettings settings)
        {

            auto bsdf1 = jpctracer::ShaderBind(jpctracer::DebugBsdf, settings.GetTexture("Color"));
            auto bsdf2 = jpctracer::ShaderBind(jpctracer::DebugBsdf, settings.GetColor("Color2"));
            return jpctracer::ShaderBind(jpctracer::MixBsdf,bsdf1, bsdf2,0.6);
        },
        {
            {"Color",""},
            {"Color2",jpctracer::FromRGB({1,1,0})}
        }
    );

    renderer.MaterialLib.Register(
        "ColorOnly",
        [](jpctracer::MaterialSettings settings)
        {
            return jpctracer::ShaderBind(jpctracer::DebugBsdf, settings.GetColor("Color"));
        },
        {
            {"Color",jpctracer::FromRGB({1,1,0})}
        }
        );

    auto shader1 = renderer.MaterialLib.Get("Default");
    //chris
    shader1->SetTexture("Color","H:\\dev\\path-tracing\\V2\\JPCPathTracer\\resource\\color_grid.png");
    auto shader2 = renderer.MaterialLib.Get("ColorOnly");
    auto triangle = jpctracer::CreateTriangle({-1,1,-2}, {1,-1,-2}, {1,1,-2});

    triangle->MaterialSlots[0] = shader1;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0,0,0}, jpctracer::FromRGB({10,10,10}));

    //Chris
    renderer.Render(1920, 1080, "");

    return 0;
}