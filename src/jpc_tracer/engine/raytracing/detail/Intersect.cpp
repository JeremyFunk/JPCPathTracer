#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/NaiveIntersection.h"
#include <optional>

namespace jpctracer::raytracing {
    IntersectionResult Intersect(const Scene& scene,const Ray& ray,ShadePrograms& programs) 
    {
        if(scene.static_bvh_type==StaticBVHType::NAIVE)
        {
            return NaiveInstancesIntersect(programs, scene,ray);
        }
        return {std::nullopt,false};
    }



}