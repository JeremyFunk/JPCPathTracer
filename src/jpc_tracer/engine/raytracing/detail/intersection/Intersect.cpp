#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/NaiveIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/scene/PrivateScene.h"
#include "jpc_tracer/engine/raytracing/detail/geometry/PrivateRootGeometry.h"
#include <optional>

namespace jpctracer::raytracing {
    IntersectionResult Intersect(const PrivateScene* scene,const Ray& ray,ShadePrograms& programs) 
    {
        if(scene->GeometryRoot.instance_bvh_type==InstanceBVHType::NAIVE)
        {
            return NaiveIntersect(programs, scene->GeometryRoot.StaticGeometries, ray);
        }
        return {std::nullopt,false};
    }



}