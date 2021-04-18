#include "BVHInetersection.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/Intersect.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include <array>



namespace jpctracer::raytracing
{
    template<class MeshT>
    IntersectionResult NaiveMeshTreeTraversal(Ray& ray, const BVHTree& tree, const MeshT& mesh, AnyHitCallBack any_hit_program, const int* material_per_slot);

    template<>
    IntersectionResult NaiveMeshTreeTraversal(Ray& ray, const BVHTree& tree, const TriangleMesh& mesh, AnyHitCallBack any_hit_program, const int* material_per_slot);

    template<>
    IntersectionResult NaiveMeshTreeTraversal(Ray& ray, const BVHTree& tree, const SphereMesh& mesh, AnyHitCallBack any_hit_program, const int* material_per_slot);

    IntersectionResult BVHStaticIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program) 
    {
        if (scene.static_instance_tree.internal_nodes.size() == 0)
        {
            const int* material_per_slot = &scene.static_instances[0].first.materials_per_slot[0];

            Ray mesh_ray = Apply(scene.static_instances[0].second, ray);

            if (scene.triangle_meshs.size() == 0)
                return NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[0], scene.sphere_meshs[0], any_hit_program, material_per_slot);
            else 
                return NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[0], scene.triangle_meshs[0], any_hit_program, material_per_slot);
        }

        const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
        const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

        std::optional<SurfaceInteraction> closest_interaction;

        int to_visit = 0;
        int current_idx = 0;
        std::array<int, 64> nodes_to_visit;

        while(true)
        {
            if(BoundsIntersect(scene.static_instance_tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
            {
                // Bound Intersect

                const auto& first_idx = scene.static_instance_tree.internal_nodes[current_idx].first_idx;
                const auto& last_idx = scene.static_instance_tree.internal_nodes[current_idx].last_idx;
                const auto& split_idx = scene.static_instance_tree.internal_nodes[current_idx].split_idx;

                IntersectionResult instance_result;

                //right leaf
                if(last_idx == split_idx + 1)
                {
                    // travers mesh tree
                    const int* material_per_slot = &scene.static_instances[last_idx].first.materials_per_slot[0];

                    Ray mesh_ray = Apply(scene.static_instances[last_idx].second, ray);

                    const size_t number_triangle_meshs = scene.triangle_meshs.size();

                    if (last_idx < number_triangle_meshs)
                    {
                        // triangle
                        instance_result = NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[last_idx], scene.triangle_meshs[last_idx], any_hit_program, material_per_slot);
                    }
                    else
                    {
                        // sphere
                        instance_result = NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[last_idx], scene.sphere_meshs[last_idx - number_triangle_meshs], any_hit_program, material_per_slot);
                    }
                    
                    if(instance_result.ShouldTerminate) return {std::nullopt,true};

                    closest_interaction = ClosestInteraction(closest_interaction, instance_result.interaction);

                    if (closest_interaction) if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx+1;
                }
             
                // left leaf
                if(first_idx == split_idx)
                {
                    // travers mesh tree
                    const int* material_per_slot = &scene.static_instances[first_idx].first.materials_per_slot[0];

                    Ray mesh_ray = Apply(scene.static_instances[first_idx].second, ray);

                    const size_t number_triangle_meshs = scene.triangle_meshs.size();

                    if (first_idx < number_triangle_meshs)
                    {
                        // triangle
                        instance_result = NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[first_idx], scene.triangle_meshs[first_idx], any_hit_program, material_per_slot);
                    }
                    else
                    {
                        // sphere
                        instance_result = NaiveMeshTreeTraversal(mesh_ray, scene.static_mesh_tree[first_idx], scene.sphere_meshs[first_idx - number_triangle_meshs], any_hit_program, material_per_slot);
                    }
                    
                    if(instance_result.ShouldTerminate) return {std::nullopt,true};

                    closest_interaction = ClosestInteraction(closest_interaction, instance_result.interaction);

                    if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx;
                }
            }

            if(to_visit == 0) break;
                
            current_idx = nodes_to_visit[--to_visit];
        }

        return {closest_interaction,false};
    }

    /*
    *   Triangle
    */

    template<>
    IntersectionResult NaiveMeshTreeTraversal(Ray& ray, const BVHTree& tree, const TriangleMesh& mesh, AnyHitCallBack any_hit_program, const int* material_per_slot)
    {
        if (tree.internal_nodes.size() == 0)
        {
            // intersect triangle
            auto interaction = Intersect(mesh, 0, ray, material_per_slot);
            if (interaction)
            {
                AnyHitResult any_hit_result = any_hit_program(*interaction);

                if (any_hit_result.ShouldTerminate)
                    return IntersectionResult{ std::nullopt,true };
            }

            return { interaction,false };
        }

        const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
        const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

        std::optional<SurfaceInteraction> closest_interaction;

        int to_visit = 0;
        int current_idx = 0;
        std::array<int, 64> nodes_to_visit;

        while(true)
        {
            if(BoundsIntersect(tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
            {
                // Bound Intersect

                const auto& first_idx = tree.internal_nodes[current_idx].first_idx;
                const auto& last_idx = tree.internal_nodes[current_idx].last_idx;
                const auto& split_idx = tree.internal_nodes[current_idx].split_idx;

                IntersectionResult instance_result;

                //right leaf
                if(last_idx == split_idx + 1)
                {
                    // intersect triangle
                    auto interaction = Intersect(mesh, last_idx, ray, material_per_slot);
                    if(interaction)
                    {
                        AnyHitResult any_hit_result = any_hit_program(*interaction);

                        if(any_hit_result.ShouldTerminate)
                            return IntersectionResult{std::nullopt,true};
                        if(any_hit_result.IsHit)
                        {
                            std::optional<SurfaceInteraction> new_interaction = ClosestInteraction(closest_interaction,interaction);
                            closest_interaction = new_interaction;
                        }
                    }

                    if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx+1;
                }
             
                // left leaf
                if(first_idx == split_idx)
                {
                    // intersect triangle
                    auto interaction = Intersect(mesh, first_idx, ray, material_per_slot);
                    if(interaction)
                    {
                        AnyHitResult any_hit_result = any_hit_program(*interaction);

                        if(any_hit_result.ShouldTerminate) return IntersectionResult{std::nullopt,true};

                        if(any_hit_result.IsHit)
                        {
                            std::optional<SurfaceInteraction> new_interaction = ClosestInteraction(closest_interaction,interaction);
                            closest_interaction = new_interaction;
                        }
                    }

                    if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx;
                }
            }

            if(to_visit == 0) break;
                
            current_idx = nodes_to_visit[--to_visit];
        }

        return {closest_interaction,false};
    }

    /*
    *   Sphere
    */

    template<>
    IntersectionResult NaiveMeshTreeTraversal(Ray& ray, const BVHTree& tree, const SphereMesh& mesh, AnyHitCallBack any_hit_program, const int* material_per_slot)
    {
        if (tree.internal_nodes.size() == 0)
        {
            // intersect triangle
            auto interaction = Intersect(mesh, 0, ray, material_per_slot);
            if (interaction)
            {
                AnyHitResult any_hit_result = any_hit_program(*interaction);

                if (any_hit_result.ShouldTerminate)
                    return IntersectionResult{ std::nullopt,true };
            }

            return { interaction,false };
        }

        const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
        const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

        std::optional<SurfaceInteraction> closest_interaction;

        int to_visit = 0;
        int current_idx = 0;
        std::array<int, 64> nodes_to_visit;

        while(true)
        {
            if(BoundsIntersect(tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
            {
                // Bound Intersect

                const auto& first_idx = tree.internal_nodes[current_idx].first_idx;
                const auto& last_idx = tree.internal_nodes[current_idx].last_idx;
                const auto& split_idx = tree.internal_nodes[current_idx].split_idx;

                IntersectionResult instance_result;

                //right leaf
                if(last_idx == split_idx + 1)
                {
                    // intersect triangle
                    auto interaction = Intersect(mesh, last_idx, ray, material_per_slot);
                    if(interaction)
                    {
                        AnyHitResult any_hit_result = any_hit_program(*interaction);

                        if(any_hit_result.ShouldTerminate)
                            return IntersectionResult{std::nullopt,true};
                        if(any_hit_result.IsHit)
                        {
                            std::optional<SurfaceInteraction> new_interaction = ClosestInteraction(closest_interaction,interaction);
                            closest_interaction = new_interaction;
                        }
                    }

                    if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx+1;
                }
             
                // left leaf
                if(first_idx == split_idx)
                {
                    // intersect triangle
                    auto interaction = Intersect(mesh, first_idx, ray, material_per_slot);
                    if(interaction)
                    {
                        AnyHitResult any_hit_result = any_hit_program(*interaction);

                        if(any_hit_result.ShouldTerminate) return IntersectionResult{std::nullopt,true};
                        
                        if(any_hit_result.IsHit)
                        {
                            std::optional<SurfaceInteraction> new_interaction = ClosestInteraction(closest_interaction,interaction);
                            closest_interaction = new_interaction;
                        }
                    }

                    if (closest_interaction) ray.ClipEnd = closest_interaction->Distance;
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx;
                }
            }

            if(to_visit == 0) break;
                
            current_idx = nodes_to_visit[--to_visit];
        }

        return {closest_interaction,false};
    }
}