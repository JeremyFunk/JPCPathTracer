#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/ReworkBVHIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/NaiveIntersection.h"
#include <optional>

namespace jpctracer::raytracing
{
IntersectionResult Intersect(const Scene& scene, Ray& ray, AnyHitCallBack any_hit_program)
{
    if (scene.static_bvh_type == StaticBVHType::NAIVE)
    {
        return NaiveInstancesIntersect(any_hit_program, scene, ray);
    }
    else if (scene.static_bvh_type == StaticBVHType::LBVH)
    {
        return BVHIntersect(ray, scene, any_hit_program);
    }

    return {std::nullopt, false};
}

} // namespace jpctracer::raytracing