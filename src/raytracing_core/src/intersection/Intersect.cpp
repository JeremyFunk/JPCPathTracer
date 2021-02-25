#include "Intersect.h"
#include "raytracing_core/include/Base.h"
#include "raytracing_core/include/Geometry.h"
#include "raytracing_core/src/intersection/acceleration/NaiveIntersection.h"
#include "raytracing_core/src/scene/PrivateScene.h"
#include "raytracing_core/src/geometry/PrivateRootGeometry.h"
#include <optional>

namespace jpc_rt_core {
    IntersectionResult Intersect(const PrivateScene* scene,const Ray& ray,ShadePrograms& programs) 
    {
        if(scene->GeometryRoot.instance_bvh_type==InstanceBVHType::NAIVE)
        {
            return NaiveIntersect(programs, scene->GeometryRoot.StaticGeometries, ray);
        }
        return {std::nullopt,false};
    }



}