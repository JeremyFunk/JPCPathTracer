#pragma once
#include "../Scene.h"
#include "IntersectionInfo.h"


namespace jpctracer::raytracing 
{
    template<class MeshT> //MeshT == TriangleMesh || MeshT == SphereMesh
    IntersectionResult NaiveIntersect(AnyHitCallBack any_hit_program,
        const MeshT& mesh,const Ray& ray,const int* material_per_slot);

    IntersectionResult NaiveInstancesIntersect(AnyHitCallBack any_hit_program,
        const Scene& scene,const Ray& ray);
}