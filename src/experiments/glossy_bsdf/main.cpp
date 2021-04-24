#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"
#include "jpc_tracer/plugins/shaders/LambertianBsdf.h"
#include <memory>

struct GlossyMaterial
{
    jpctracer::Spectrum color;
    jpctracer::Prec roughness;
    jpctracer::Prec glossy_influence;
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            auto bsdf_g = jpctracer::GlossyBsdf(scattered, jpctracer::FromRGB({1, 1, 1}), roughness);
            auto bsdf_d = jpctracer::DiffuseBsdf(scattered, color);
            return jpctracer::Mix(bsdf_g, bsdf_d, glossy_influence);
        };
    }
};

int main()
{
    using namespace jpctracer;
    Logger::Init();
    using sampler_t = decltype(sampler::StratifiedSamplerFast());
    std::unique_ptr<ISampler> sampler = std::make_unique<sampler_t>(sampler::StratifiedSamplerFast());

    std::unique_ptr<ICamera> camera = std::make_unique<camera::ProjectionCamera>(1);

    std::unique_ptr<IIntegrator> integrator = std::make_unique<DirectLightIntegrator>(4, 4);

    JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.TileSize = 64;
    renderer.ShouldMultiThread = false;

    auto shader = renderer.MaterialLib.Create<GlossyMaterial>();
    std::cout << "Set roughness\n";
    float roughness = 0;
    float glossy_influence = 0;
    std::cin >> roughness;
    std::cout << "Set glossy influence\n";
    std::cin >> glossy_influence;
    shader->roughness = roughness;
    shader->color = FromRGB({0.4, 0.1, 0.7});
    shader->glossy_influence = glossy_influence;

    // chris
    auto triangle = CreateTriangle({-1, 1, -2}, {1, -1, -2}, {1, 1, -2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0, 0, 0}, FromRGB({10, 10, 10}));

    // Chris
    renderer.Render(500, 300, "");

    return 0;
}