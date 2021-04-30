#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <stdint.h>

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/raytracing/detail/Intersect.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"

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

namespace jpctracer::raytracing
{

TEST(LBVH, LBVHBuildTest)
{
    SceneBuilder builder;

    auto mesh =
        std::get<TriangleMesh>(jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\susan.obj")->mesh);

    std::cout << mesh.Vertices.size() << ' ' << mesh.TriangleGeometries.size() << '\n';

    auto id = builder.AddMesh(std::move(mesh));

    builder.AddInstance(id);

    AccelerationSettings settings = {raytracing::DynamicBVHType::NAIVE, raytracing::StaticBVHType::LBVH};

    auto scene = builder.Build(settings);
}

TEST(LBVH, LBVHIntersectTest)
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSamplerFast());
    std::unique_ptr<jpctracer::ISampler> sampler =
        std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSamplerFast());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4, 2);
    // std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DebugIntegrator>();

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));
    renderer.ShouldMultiThread = false;

    auto shader = renderer.MaterialLib.Create<DebugMaterial>();

    shader->color = jpctracer::FromRGB({0.4, 0.6, 1});

    // Peer
    auto susan = jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\Susan.obj");

    susan->transformation = jpctracer::RotScalTrans({0, 0, -3}, 1, {0, 0, 0});
    susan->MaterialSlots[0] = shader;

    renderer.Draw(susan);
    renderer.LightsLib.AddPointLight({-2, 5, 1}, jpctracer::FromRGB({1, 1, 1}) * 500);

    renderer.Acceleration = {jpctracer::raytracing::DynamicBVHType::NAIVE, jpctracer::raytracing::StaticBVHType::LBVH};

    renderer.Render(1, 1, "");
}
} // namespace jpctracer::raytracing