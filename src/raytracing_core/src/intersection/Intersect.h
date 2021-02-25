#pragma once
#include "raytracing_core/include/Scene.h"
#include "raytracing_core/src/scene/PrivateScene.h"
#include "raytracing_core/src/shade_programs/ShadePrograms.h"
#include "raytracing_core/src/intersection/acceleration/IntersectionInfo.h"

namespace jpc_rt_core {
    
    IntersectionResult Intersect(const PrivateScene* scene,const Ray& ray,ShadePrograms& programs);
}