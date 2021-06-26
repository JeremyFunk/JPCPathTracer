#pragma once

#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"

namespace jpctracer::raytracing
{

IntersectionResult InstanceBVHIntersect(Ray& ray, const BVHTree& tree, const Scene& scene,
                                        AnyHitCallBack any_hit_program);

}