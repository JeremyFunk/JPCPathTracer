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
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHBuilderHelper.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHConstructionHelper.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include <memory>

namespace jpctracer::raytracing
{
std::unique_ptr<Scene> build_scene(StaticBVHType type, std::string triangle_mesh_path)
{
    SceneBuilder builder;

    auto mesh = std::get<TriangleMesh>(jpctracer::LoadMesh(triangle_mesh_path)->mesh);

    std::cout << mesh.Vertices.size() << ' ' << mesh.TriangleGeometries.size() << '\n';

    auto id = builder.AddMesh(std::move(mesh));

    auto instance_id = builder.AddInstance(id, RotScalTrans({0, 0, -3}, 1, {0, 0, 0}));

    builder.MaterialBind(instance_id, 0, 0);

    AccelerationSettings settings = {raytracing::DynamicBVHType::NAIVE, type};
    auto scene = builder.Build(settings);

    return scene;
}
TEST(LBVH, LBVHBuildTest)
{
    std::string path_triangle_mesh = "E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\cube.obj";

    auto scene_lbvh = build_scene(StaticBVHType::LBVH, path_triangle_mesh);

    auto scene_naive = build_scene(StaticBVHType::NAIVE, path_triangle_mesh);
    // sort NAIVE by morton code
    auto& triangle = scene_naive->triangle_meshs[0];
    auto morton_codes = GenerateTriangleMortonCodes(triangle);
    SortTriangleByMortonCode(triangle, morton_codes);

    Ray ray;

    ray.Origin = {0, 0, 0};
    ray.Direction = {0, 0, -0.8};

    auto any_hit_func = [](const SurfaceInteraction& interaction) -> AnyHitResult { return {true, false}; };

    auto intersect_result_lbvh = Intersect(*scene_lbvh, ray, any_hit_func);

    auto intersect_result_naive = Intersect(*scene_naive, ray, any_hit_func);

    EXPECT_EQ(intersect_result_lbvh.interaction.has_value(), intersect_result_naive.interaction.has_value());

    for (int i = 0; i < 3; i++)
        EXPECT_EQ(intersect_result_lbvh.interaction->Point[i], intersect_result_naive.interaction->Point[i]);
}

TEST(LBVH, LBVHIntersectTest)
{
    std::string path_triangle_mesh = "E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\Susan.obj";

    auto scene_lbvh = build_scene(StaticBVHType::LBVH, path_triangle_mesh);

    auto scene_naive = build_scene(StaticBVHType::NAIVE, path_triangle_mesh);
    // sort NAIVE by morton code
    auto& triangle = scene_naive->triangle_meshs[0];
    auto morton_codes = GenerateTriangleMortonCodes(triangle);
    SortTriangleByMortonCode(triangle, morton_codes);

    Ray ray;

    ray.Origin = {0, 0, 0};
    ray.Direction = {0, 0, -0.8};

    auto any_hit_func = [](const SurfaceInteraction& interaction) -> AnyHitResult { return {true, false}; };

    auto intersect_result_lbvh = Intersect(*scene_lbvh, ray, any_hit_func);

    auto intersect_result_naive = Intersect(*scene_naive, ray, any_hit_func);

    EXPECT_EQ(intersect_result_lbvh.interaction.has_value(), intersect_result_naive.interaction.has_value());

    for (int i = 0; i < 3; i++)
        EXPECT_EQ(intersect_result_lbvh.interaction->Point[i], intersect_result_naive.interaction->Point[i]);
}
} // namespace jpctracer::raytracing