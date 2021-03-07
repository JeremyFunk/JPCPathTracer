#pragma once
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/detail/scene/PrivateScene.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadePrograms.h"
#include "jpc_tracer/raytracing/detail/intersection/acceleration/IntersectionInfo.h"

namespace jpctracer::raytracing {
    
    IntersectionResult Intersect(const PrivateScene* scene,const Ray& ray,ShadePrograms& programs);
}