#pragma once
#include "jpc_tracer/engine/raytracing/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/scene/PrivateScene.h"
#include "jpc_tracer/engine/raytracing/detail/shade_programs/ShadePrograms.h"
#include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/IntersectionInfo.h"

namespace jpctracer::raytracing {
    
    IntersectionResult Intersect(const PrivateScene* scene,const Ray& ray,ShadePrograms& programs);
}