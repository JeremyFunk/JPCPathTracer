#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/integrators/DebugIntegrator.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"
#include "jpc_tracer/plugins/shaders/LambertianBsdf.h"
#include <memory>

struct Material1
{
    jpctracer::Spectrum color = jpctracer::FromRGB({0, 0, 1});
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            auto b1 = jpctracer::DiffuseBsdf(scattered, color);
            auto b2 = jpctracer::GlossyBsdf(scattered, jpctracer::FromValue(1), 0.01);
            return jpctracer::Mix(b1, b2, 0.6);
        };
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
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            return jpctracer::DiffuseBsdf(scattered, jpctracer::FromRGB({1, 1, 1}));
        };
    }
};

static_assert(jpctracer::JPCShader<Material1>);

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSampler());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4, 1);
    // std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DebugIntegrator>();

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.ShouldMultiThread = true;

    auto shader = renderer.MaterialLib.Create<Material1>();

    shader->color = jpctracer::FromRGB({0.4, 0.6, 1});

    auto triangle = jpctracer::CreateTriangle({-5, 5, -5}, {5, -5, -5}, {5, 5, -5});

    triangle->MaterialSlots[0] = shader;

    auto cube_shader = renderer.MaterialLib.Create<Material1>();

    cube_shader.BindTexture(&cube_shader->color,
                            // "/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/color_grid.png" /* Christian */);
                            "E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\color_grid.png" /* Peer */);

    // auto sphere_shader = renderer.MaterialLib.Create<Glossy>();
    auto sphere_shader = renderer.MaterialLib.Create<Material1>();

    auto sphere = jpctracer::CreateSphere({-2, 0, -4}, 1);
    sphere->MaterialSlots[0] = sphere_shader;

    // auto cube = jpctracer::LoadMesh("/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/Susan.obj");

    // Christian
    // auto cube = jpctracer::LoadMesh("/home/chris/Dev/path_tracing/V2/JPCPathTracer/resource/cube.obj");
    // Peer
    auto cube = jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\cube.obj");

    cube->transformation = jpctracer::RotScalTrans({0, 0, -5}, 1, {0, 0, 0});
    cube->MaterialSlots[0] = cube_shader;

    renderer.Draw(triangle);
    // renderer.Draw(sphere);
    renderer.Draw(cube);
    renderer.LightsLib.AddPointLight({-2, 5, -4}, jpctracer::FromRGB({1, 1, 1}) * 500);

    // Peer
    renderer.Acceleration = {jpctracer::raytracing::DynamicBVHType::NAIVE, jpctracer::raytracing::StaticBVHType::NAIVE};
    renderer.ShouldMultiThread = false;

    // Chris
    renderer.Render(100, 100, "");

    return 0;
}