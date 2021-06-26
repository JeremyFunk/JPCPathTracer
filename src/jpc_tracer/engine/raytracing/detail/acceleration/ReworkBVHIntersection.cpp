
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/InstanceBVHIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/MeshBVHIntersection.h"

namespace jpctracer::raytracing
{

// Traverse all needed Scene BVHs
IntersectionResult BVHIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program)
{
    auto& tree = scene.static_instance_tree;

    return InstanceBVHIntersect(ray, tree, scene, any_hit_program);
}

} // namespace jpctracer::raytracing