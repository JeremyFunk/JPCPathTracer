#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BVHIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/NaiveIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/ApplyTransformation.h"
#include <optional>

namespace jpctracer::raytracing {
    IntersectionResult Intersect(const Scene& scene, Ray& ray,
        AnyHitCallBack any_hit_program) 
    {
        if(scene.static_bvh_type==StaticBVHType::NAIVE)
        {
            return NaiveInstancesIntersect(any_hit_program, scene,ray);
        }
        else if (scene.static_bvh_type == StaticBVHType::LBVH)
        {
            // LBVH TREE
            auto intersect_func = [&scene] (const Ray& ray, const int& instance_idx, AnyHitCallBack any_hit_program)
                                {
                                    // Mesh intersection function
                                    auto mesh_intesect_func = [&scene, &instance_idx](const Ray& ray, const int& mesh_idx, AnyHitCallBack any_hit_program) 
                                        { 
                                            const size_t& number_triangle_meshs = scene.triangle_meshs.size();

                                            if (instance_idx < number_triangle_meshs)
                                            {
                                                // Triangle
                                                return IntersectMesh<TriangleMesh>(ray, scene.triangle_meshs[instance_idx], mesh_idx, &scene.static_instances[instance_idx].first.materials_per_slot[0], any_hit_program); 
                                            }

                                            // Sphere
                                            return IntersectMesh<SphereMesh>(ray, scene.sphere_meshs[instance_idx-number_triangle_meshs], mesh_idx, &scene.static_instances[instance_idx].first.materials_per_slot[0], any_hit_program); 
                                        };

                                    Ray mesh_ray = ApplyTransformation(ray, scene.static_instances[instance_idx].second);

                                    // Intersect Mesh Tree
                                    return BVHIntersect(mesh_ray, scene.static_mesh_tree[instance_idx], any_hit_program, mesh_intesect_func);
                                };

            return BVHIntersect(ray, scene.static_instance_tree, any_hit_program, intersect_func);
            // return BVHStaticIntersect(ray, scene, any_hit_program);
        }

        return {std::nullopt,false};
    }



}