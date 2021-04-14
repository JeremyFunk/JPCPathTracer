#pragma once

#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
namespace jpctracer::raytracing
{

    struct Instance
    {
        MeshId mesh_id;
        std::vector<int> materials_per_slot;
    };

    struct Scene
    {
        StaticBVHType static_bvh_type = StaticBVHType::NAIVE;
        DynamicBVHType dynamic_bvh_type = DynamicBVHType::NAIVE;

        std::vector<std::pair<Instance,Transformation>> static_instances;
        std::vector<TriangleMesh> triangle_meshs;
        std::vector<SphereMesh> sphere_meshs;
    };
}