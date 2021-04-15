#pragma once
#include "Scene.h"
#include "acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/Base.h"

namespace jpctracer::raytracing {
    
    IntersectionResult Intersect(const Scene& scene,const Ray& ray,
        AnyHitCallBack any_hit_program);
}