#pragma once
#include "../Scene.h"
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"

namespace jpctracer::raytracing
{
template <class MeshT> // MeshT == TriangleMesh || MeshT == SphereMesh
IntersectionResult NaiveIntersect(AnyHitCallBack any_hit_program, const MeshT& mesh, const Ray& ray,
                                  const int* material_per_slot, const Transformation& trans);

IntersectionResult NaiveInstancesIntersect(AnyHitCallBack any_hit_program, const Scene& scene, const Ray& ray);
} // namespace jpctracer::raytracing