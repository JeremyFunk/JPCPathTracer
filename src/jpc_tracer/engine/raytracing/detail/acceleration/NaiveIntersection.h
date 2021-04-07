#pragma once
#include "../Scene.h"
#include "IntersectionInfo.h"
#include "../ShadePrograms.h"


namespace jpctracer::raytracing 
{
    template<class MeshT> //MeshT == TriangleMesh || MeshT == SphereMesh
    IntersectionResult NaiveIntersect(ShadePrograms& programs,const MeshT& mesh,const Ray& ray,const int* material_per_slot);

    IntersectionResult NaiveInstancesIntersect(ShadePrograms& programs, const Scene& scene,const Ray& ray);
}