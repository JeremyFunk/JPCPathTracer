#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <stdint.h>

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"

namespace jpctracer::raytracing {
    TEST(LBVH, LBVHBuildTest)
    {
        SceneBuilder builder;

        auto mesh = std::get<TriangleMesh>(jpctracer::LoadMesh("E:\\dev\\pathTrace\\V2\\JPCPathTracer\\resource\\cube.obj")->mesh);

        std::cout << mesh.Vertices.size() << ' ' << mesh.TriangleGeometries.size() << '\n';

        auto id = builder.AddMesh(std::move(mesh));

        builder.AddInstance(id);

        AccelerationSettings settings = {raytracing::DynamicBVHType::NAIVE, raytracing::StaticBVHType::LBVH};

        auto scene = builder.Build(settings);
    }
}