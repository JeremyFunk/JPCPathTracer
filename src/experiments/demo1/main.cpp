#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/integrators/DebugIntegrator.h"
#include "jpc_tracer/plugins/integrators/DirectLightIntegrator.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"
#include "jpc_tracer/plugins/shaders/LambertianBsdf.h"
#include <memory>

struct Material1
{
    jpctracer::Spectrum color = jpctracer::FromRGB({1, 1, 1});
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            scattered.ClipEnd = 0.f;
            auto b1 = jpctracer::DiffuseBsdf(scattered, color);
            auto b2 = jpctracer::GlossyBsdf(scattered, jpctracer::FromValue(1), 0.01);
            return jpctracer::Mix(b1, b2, 0.6);
        };
    }
};

struct DiffuseMat
{
    jpctracer::Spectrum color = jpctracer::FromRGB({1, 1, 1});
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) { return jpctracer::DiffuseBsdf(scattered, color); };
    }
};

struct Glossy
{
    jpctracer::Spectrum color = jpctracer::FromRGB({1, 1, 1});
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) { return jpctracer::GlossyBsdf(scattered, color, 0.01); };
    }
};

struct Material2
{
    jpctracer::Spectrum color = jpctracer::FromRGB({1, 1, 1});
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) { return jpctracer::DebugBsdf(color); };
    }
};

static_assert(jpctracer::JPCShader<Material1>);

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSamplerFast());
    std::unique_ptr<jpctracer::ISampler> sampler =
        std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSamplerFast());

    // using sampler_t = decltype(jpctracer::sampler::DebugSampler());
    // std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::DebugSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator =
        std::make_unique<jpctracer::PathIntegrator>(4 /*lights*/, 32 /*sub pixels*/, 4 /*depth*/);
    // std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DebugIntegrator>();
    // std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4, 1);

    jpctracer::JPCRenderer jpc_renderer(std::move(sampler), std::move(camera), std::move(integrator));
    jpc_renderer.ShouldMultiThread = true;

    auto shader = jpc_renderer.MaterialLib.Create<DiffuseMat>();

    shader->color = jpctracer::FromRGB({1, 1, 1});

    auto triangle = jpctracer::CreateTriangle({-5, 5, -5}, {5, -5, -5}, {5, 5, -5});

    triangle->MaterialSlots[0] = shader;

    auto test_shader = jpc_renderer.MaterialLib.Create<DiffuseMat>();
    test_shader.BindTexture(&test_shader->color,
                            // "/home/chris/dev/path_tracing/V2/JPCPathTracer/resource/color_grid.png" /* Christian */
                            "E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\color_grid.png" /* Peer */
    );

    auto sphere_shader = jpc_renderer.MaterialLib.Create<Glossy>();

    auto sphere = jpctracer::CreateSphere({0, 0, -3}, 1);
    sphere->MaterialSlots[0] = sphere_shader;

    // auto cube = jpctracer::LoadMesh("/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/Susan.obj");

    // Christian
    // auto cube = jpctracer::LoadMesh("/home/chris/dev/path_tracing/V2/JPCPathTracer/resource/cube.obj");
    // Peer
    auto cube = jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\cube.obj");

    cube->transformation = jpctracer::TransScalRot({0, -2, -3}, 1, {0, 0, 0});
    cube->MaterialSlots[0] = test_shader;

    jpc_renderer.Draw(triangle);
    jpc_renderer.Draw(sphere);
    jpc_renderer.Draw(cube);
    jpc_renderer.LightsLib.AddPointLight({-2, 5, -4}, jpctracer::FromRGB({0.2, 0.1, 0.7}) * 50);
    // renderer.LightsLib.AddPointLight({-2, 5, 1}, jpctracer::FromValue(1) * 50);
    jpc_renderer.LightsLib.AddDistanceLight({0, -1, -1}, jpctracer::FromValue(1));
    // renderer.LightsLib.AddPointLight({-4, 5, -4}, jpctracer::FromRGB({0.6, 0.2, 0.7}) * 10);

    // Peer
    jpc_renderer.Acceleration = {jpctracer::raytracing::DynamicBVHType::NAIVE,
                                 jpctracer::raytracing::StaticBVHType::LBVH};

    // Chris
    jpc_renderer.Render(300, 600, "");

    return 0;
}
