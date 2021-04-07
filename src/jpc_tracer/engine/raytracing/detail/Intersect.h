#pragma once
#include "Scene.h"
#include "ShadePrograms.h"
#include "acceleration/IntersectionInfo.h"

namespace jpctracer::raytracing {
    
    IntersectionResult Intersect(const Scene& scene,const Ray& ray,ShadePrograms& programs);
}