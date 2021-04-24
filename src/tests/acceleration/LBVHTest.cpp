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
        std::get<TriangleMesh>(jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\cube.obj")->mesh);

    std::cout << mesh.Vertices.size() << ' ' << mesh.TriangleGeometries.size() << '\n';

    auto id = builder.AddMesh(std::move(mesh));

    builder.AddInstance(id);

    AccelerationSettings settings = {raytracing::DynamicBVHType::NAIVE, raytracing::StaticBVHType::LBVH};

    auto scene = builder.Build(settings);
}

TEST(LBVH, LBVHIntersectTest)
{
    jpctracer::Logger::Init();
    using sampler_t = decltype(jpctracer::sampler::StratifiedSampler());
    std::unique_ptr<jpctracer::ISampler> sampler = std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSampler());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4, 1);

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));

    auto shader = renderer.MaterialLib.Create<DebugMaterial>();
    shader->color = FromRGB({1, 0, 1});
    auto triangle = jpctracer::CreateTriangle({-1, 1, -2}, {1, -1, -2}, {1, 1, -2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0, 0, 0}, jpctracer::FromRGB({1, 1, 1}));

    // Chris
    renderer.Render(300, 300, "");
}
} // namespace jpctracer::raytracing