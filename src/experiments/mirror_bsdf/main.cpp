#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
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
#include <memory>

struct MirrorMaterial
{
    auto bsdf()
    {
        return jpctracer::MirrorBsdf;
    };
};

struct DiffuseMaterial
{
    auto bsdf()
    {
        return [=](jpctracer::Ray scattered) {
            auto b = jpctracer::DebugBsdf(jpctracer::FromRGB({0.5, 0.7, 0.3}));
            b.emission = jpctracer::FromRGB({0.5, 0.7, 0.3});
            return b;
        };
    };
};

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::DebugSampler());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::DebugSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::PathIntegrator>(4, 1, 2);

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.ShouldMultiThread = false;

    auto shader = renderer.MaterialLib.Create<MirrorMaterial>();
    auto triangle = jpctracer::CreateTriangle({-1, 1, -2}, {1, -1, -2}, {1, 1, -2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);

    auto cube = jpctracer::LoadMesh("/home/chris/dev/path_tracing/V2/JPCPathTracer/resource/cube.obj");

    auto cube_shader = renderer.MaterialLib.Create<DiffuseMaterial>();
    cube->MaterialSlots[0] = cube_shader;

    cube->transformation = jpctracer::TransScalRot({-0.5, 0, -1}, 0.2, {0, 0, 0});
    renderer.Draw(cube);

    renderer.LightsLib.AddPointLight({0, 0, 0}, jpctracer::FromRGB({1, 1, 1}));

    // Chris
    renderer.Render(300, 300, "");

    return 0;
}
