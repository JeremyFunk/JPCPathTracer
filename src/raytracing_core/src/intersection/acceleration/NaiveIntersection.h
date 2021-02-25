#pragma once
#include "raytracing_core/include/Scene.h"
#include "raytracing_core/src/scene/PrivateScene.h"
#include "raytracing_core/src/geometry/PrivateRootGeometry.h"
#include "raytracing_core/src/geometry/PrivateGeometry.h"
#include "raytracing_core/src/intersection/acceleration/IntersectionInfo.h"


namespace jpc_rt_core 
{
    IntersectionResult NaiveTriangleIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray);
    IntersectionResult NaiveSphereIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray);
    IntersectionResult NaiveIntersect(ShadePrograms& programs,const std::vector<PrivateGeometry>& geometries,const Ray& ray);
}