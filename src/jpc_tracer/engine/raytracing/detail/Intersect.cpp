#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/NaiveIntersection.h"
#include <optional>

namespace jpctracer::raytracing {
    IntersectionResult Intersect(const Scene& scene,const Ray& ray,
        AnyHitCallBack any_hit_program) 
    {
        if(scene.static_bvh_type==StaticBVHType::NAIVE)
        {
            return NaiveInstancesIntersect(any_hit_program, scene,ray);
        }
        return {std::nullopt,false};
    }



}