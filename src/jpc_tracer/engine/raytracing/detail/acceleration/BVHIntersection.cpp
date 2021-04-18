#include "BVHIntersection.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/ApplyTransformation.h"
#include "jpc_tracer/engine/raytracing/detail/Intersect.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include <array>



namespace jpctracer::raytracing
{
    // template <class MeshT> // SphereMesh || TriangleMesh
    // IntersectionResult IntersectMesh(const Ray& ray, const MeshT& mesh, const int& idx, const int* material_per_slot, AnyHitCallBack any_hit_program)
    // {
    //     std::optional<SurfaceInteraction> interaction = Intersect(mesh, idx, ray, material_per_slot);

    //     if(interaction)
    //     {
    //         AnyHitResult any_hit_result = any_hit_program(*interaction);

    //             if(any_hit_result.ShouldTerminate)
    //                 return IntersectionResult{std::nullopt,true};
    //     }

    //     return {interaction, false};
    // }

    // IntersectionResult BVHStaticIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program) 
    // {
    //     if (scene.static_instance_tree.internal_nodes.size() == 0)
    //     {
    //         const int* material_per_slot = &scene.static_instances[0].first.materials_per_slot[0];

    //         Ray mesh_ray = ApplyTransformation(ray, scene.static_instances[0].second);

    //         if (scene.triangle_meshs.size() == 0)
    //             return BVHIntersect(mesh_ray, scene.static_mesh_tree[0], any_hit_program, [&material_per_slot, &scene](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program) 
    //                                                                                             { return IntersectMesh(ray, scene.sphere_meshs[0], idx, material_per_slot, any_hit_program); } );
    //         else 
    //             return BVHIntersect(mesh_ray, scene.static_mesh_tree[0], any_hit_program, 
    //                 [&material_per_slot, &scene](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program)
    //                 { return IntersectMesh(ray, scene.triangle_meshs[0], idx, material_per_slot, any_hit_program); } );
    //     }

    //     const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
    //     const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

    //     std::optional<SurfaceInteraction> closest_interaction;

    //     int to_visit = 0;
    //     int current_idx = 0;
    //     std::array<int, 64> nodes_to_visit;

    //     while(true)
    //     {
    //         if(BoundsIntersect(scene.static_instance_tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
    //         {
    //             // Bound Intersect

    //             const auto& first_idx = scene.static_instance_tree.internal_nodes[current_idx].first_idx;
    //             const auto& last_idx = scene.static_instance_tree.internal_nodes[current_idx].last_idx;
    //             const auto& split_idx = scene.static_instance_tree.internal_nodes[current_idx].split_idx;

    //             IntersectionResult instance_result;

    //             //right leaf
    //             if(last_idx == split_idx + 1)
    //             {
    //                 // travers mesh tree
    //                 const int* material_per_slot = &scene.static_instances[last_idx].first.materials_per_slot[0];

    //                 Ray mesh_ray = ApplyTransformation(ray, scene.static_instances[last_idx].second);

    //                 const size_t number_triangle_meshs = scene.triangle_meshs.size();

    //                 if (last_idx < number_triangle_meshs)
    //                 {
    //                     // triangle
    //                     instance_result = BVHIntersect(mesh_ray, scene.static_mesh_tree[last_idx], any_hit_program, [&material_per_slot, &scene, &last_idx](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program) 
    //                                                                                             { return IntersectMesh(ray, scene.triangle_meshs[last_idx], idx, material_per_slot, any_hit_program); } );
    //                 }
    //                 else
    //                 {
    //                     // sphere
    //                     auto sphere_idx = last_idx - number_triangle_meshs;
    //                     instance_result = BVHIntersect(mesh_ray, scene.static_mesh_tree[0], any_hit_program, 
    //                                         [&material_per_slot, &scene, &sphere_idx](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program)
    //                                         { return IntersectMesh(ray, scene.sphere_meshs[sphere_idx], idx, material_per_slot, any_hit_program); } );
    //                 }
                    
    //                 if(instance_result.ShouldTerminate) return {std::nullopt,true};

    //                 closest_interaction = ClosestInteraction(closest_interaction, instance_result.interaction);

    //                 if (closest_interaction) if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
    //             }
    //             else
    //             {
    //                 nodes_to_visit[to_visit++] = split_idx+1;
    //             }
             
    //             // left leaf
    //             if(first_idx == split_idx)
    //             {
    //                 // travers mesh tree
    //                 const int* material_per_slot = &scene.static_instances[first_idx].first.materials_per_slot[0];

    //                 Ray mesh_ray = ApplyTransformation(ray, scene.static_instances[first_idx].second);

    //                 const size_t number_triangle_meshs = scene.triangle_meshs.size();

    //                 if (first_idx < number_triangle_meshs)
    //                 {
    //                     // triangle
    //                     instance_result = BVHIntersect(mesh_ray, scene.static_mesh_tree[first_idx], any_hit_program, [&material_per_slot, &scene, &first_idx](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program) 
    //                                                                                             { return IntersectMesh(ray, scene.triangle_meshs[first_idx], idx, material_per_slot, any_hit_program); } );
    //                 }
    //                 else
    //                 {
    //                     // sphere
    //                     auto sphere_idx = first_idx - number_triangle_meshs;
    //                     instance_result = BVHIntersect(mesh_ray, scene.static_mesh_tree[0], any_hit_program, 
    //                                         [&material_per_slot, &scene, &sphere_idx](const Ray& ray, const int& idx, AnyHitCallBack any_hit_program)
    //                                         { return IntersectMesh(ray, scene.sphere_meshs[sphere_idx], idx, material_per_slot, any_hit_program); } );
    //                 }
                    
    //                 if(instance_result.ShouldTerminate) return {std::nullopt,true};
                    
    //                 closest_interaction = ClosestInteraction(closest_interaction, instance_result.interaction);

    //                 if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
    //             }
    //             else
    //             {
    //                 nodes_to_visit[to_visit++] = split_idx;
    //             }
    //         }

    //         if(to_visit == 0) break;
                
    //         current_idx = nodes_to_visit[--to_visit];
    //     }

    //     return {closest_interaction,false};
    // }
}