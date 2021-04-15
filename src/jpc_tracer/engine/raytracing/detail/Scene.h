#pragma once

#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include <vector>
namespace jpctracer::raytracing
{

    struct Instance
    {
        MeshId mesh_id;
        std::vector<int> materials_per_slot;
    };

    struct Scene
    {
        //BVH Types
        StaticBVHType static_bvh_type = StaticBVHType::NAIVE;
        DynamicBVHType dynamic_bvh_type = DynamicBVHType::NAIVE;

        //Mesh Data
        std::vector<std::pair<Instance,Transformation>> static_instances;
        std::vector<TriangleMesh> triangle_meshs;
        std::vector<SphereMesh> sphere_meshs;

        //BVH Data
        std::vector<BVHTree> static_mesh_tree;
        BVHTree static_instance_tree;

        //intersection function ptr
        std::vector<MissFuncPtr> mis_programs;
        std::vector<AnyHitFuncPtr> any_hit_programs;
        std::vector<ClosestHitFuncPtr> closest_hit_programs;
        std::vector<const void*> materials;
        
        //counter
        int material_count;
        int ray_types_count;
    };
}