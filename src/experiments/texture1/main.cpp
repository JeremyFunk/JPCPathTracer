#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include <memory>

struct Material1
{
    jpctracer::Spectrum Color1 = jpctracer::Black();
    jpctracer::Spectrum Color2 = jpctracer::FromRGB({1, 1, 0});

    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            auto bsdf1 = jpctracer::DebugBsdf(Color1);
            auto bsdf2 = jpctracer::DebugBsdf(Color2);
            return jpctracer::Mix(bsdf1, bsdf2, 0.6);
        };
    }
};

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSampler2());
    std::unique_ptr<jpctracer::ISampler> sampler =
        std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSampler2());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(128, 4);

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.TileSize = 64;
    renderer.ShouldMultiThread = true;

    auto shader = renderer.MaterialLib.Create<Material1>();

    // chris
    // shader.BindTexture(&shader->Color1, "/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/color_grid.png");
    shader.BindTexture(&shader->Color1, "H:\\dev\\path-tracing\\V2\\JPCPathTracer\\resource\\color_grid.png");

    auto triangle = jpctracer::CreateTriangle({-1, 1, -2}, {1, -1, -2}, {1, 1, -2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0, 0, 0}, jpctracer::FromRGB({10, 10, 10}));

    renderer.Acceleration = {jpctracer::raytracing::DynamicBVHType::NAIVE, jpctracer::raytracing::StaticBVHType::LBVH};

    // Chris
    renderer.Render(1920, 1080, "");

    return 0;
}