#pragma once
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/detail/scene/PrivateScene.h"
#include "jpc_tracer/raytracing/detail/geometry/PrivateRootGeometry.h"
#include "jpc_tracer/raytracing/detail/geometry/PrivateGeometry.h"
#include "jpc_tracer/raytracing/detail/intersection/acceleration/IntersectionInfo.h"


namespace jpctracer::raytracing 
{
    IntersectionResult NaiveTriangleIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray);
    IntersectionResult NaiveSphereIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray);
    IntersectionResult NaiveIntersect(ShadePrograms& programs,const std::vector<PrivateGeometry>& geometries,const Ray& ray);
}