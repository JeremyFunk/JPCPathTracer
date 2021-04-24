#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
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

int main()
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::DebugSampler());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::DebugSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DebugIntegrator>();

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.ShouldMultiThread = false;

    auto shader = renderer.MaterialLib.Create<MirrorMaterial>();
    auto triangle = jpctracer::CreateTriangle({-1, 1, -2}, {1, -1, -2}, {1, 1, -2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0, 0, 0}, jpctracer::FromRGB({1, 1, 1}));

    // Chris
    renderer.Render(30, 30, "");

    return 0;
}