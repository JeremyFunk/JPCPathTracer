#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/renderer/Geomtry.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/engine/utilities/SamplingRoutins.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/integrators/DebugIntegrator.h"
#include "jpc_tracer/plugins/integrators/DirectLightIntegrator.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"
#include "jpc_tracer/plugins/shaders/LambertianBsdf.h"
#include "libs/google-benchmark/googlebenchmark/src/benchmark_runner.h"
#include <benchmark/benchmark.h>
#include <cmath>
#include <memory>

static constexpr int number_iter = 50;
static constexpr int number_reps = 1;

jpctracer::JPCRenderer JitterdScene(const jpctracer::uint width = 100, const jpctracer::uint height = 100,
                                    const jpctracer::uint number_spheres = 100, const jpctracer::uint sphere_radius = 1,
                                    const jpctracer::uint light_depth = 4, const jpctracer::uint sub_pixels = 32,
                                    const jpctracer::uint bounce_depth = 4, const bool should_multi_thread = true)
{
    using sampler_t = decltype(jpctracer::sampler::StratifiedSamplerFast());
    std::unique_ptr<jpctracer::ISampler> sampler =
        std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSamplerFast());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::PathIntegrator>(
        light_depth /*lights*/, sub_pixels /*sub pixels*/, bounce_depth /*depth*/);

    jpctracer::JPCRenderer jpc_renderer(std::move(sampler), std::move(camera), std::move(integrator));
    jpc_renderer.ShouldMultiThread = should_multi_thread;

    auto diffuse_shader = jpc_renderer.MaterialLib.Create<jpctracer::DebugMaterial>();

    int spheres_per_dim = std::sqrt(number_spheres);
    int x_step = width / spheres_per_dim;
    int y_step = height / spheres_per_dim;

    std::vector<jpctracer::renderer::Geometry> sphere_space;

    // generate spheres
    for (jpctracer::Prec x = 0; x < width; x += x_step)
    {
        for (jpctracer::Prec y = 0; y < height; y += y_step)
        {
            auto sample_3D = jpctracer::CosinusSampleHemisphere({x, y});

            auto sphere = jpctracer::CreateSphere(sample_3D, sphere_radius);
            sphere->MaterialSlots[0] = diffuse_shader;

            jpc_renderer.Draw(sphere);
        }
    }

    jpc_renderer.LightsLib.AddDistanceLight({0, 0, -1}, jpctracer::FromValue(1));

    jpc_renderer.Acceleration = {jpctracer::raytracing::DynamicBVHType::NAIVE,
                                 jpctracer::raytracing::StaticBVHType::LBVH};

    return jpc_renderer;
}

static void BM_TracingPerformance(benchmark::State& state)
{
    const jpctracer::uint light_depth = 1;
    const jpctracer::uint sub_pixels = 1;
    const jpctracer::uint bounce_depth = 1;

    const jpctracer::uint width = 100;
    const jpctracer::uint height = 100;

    const bool ShouldMultiThread = true;

    const jpctracer::uint number_spheres = 100;
    const jpctracer::uint sphere_radius = 1;

    for (auto _ : state)
    {
        state.PauseTiming();
        auto jpc_renderer = JitterdScene(width, height, number_spheres, sphere_radius, light_depth, sub_pixels,
                                         bounce_depth, ShouldMultiThread);
        state.ResumeTiming();

        jpc_renderer.Render(width, height, "");
    }
}

BENCHMARK(BM_TracingPerformance)
    ->UseRealTime()
    ->Repetitions(number_reps)
    ->Iterations(number_iter)
    ->Unit(benchmark::TimeUnit::kSecond);

BENCHMARK_MAIN();