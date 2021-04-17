#pragma once

#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"

namespace jpctracer::raytracing
{
    IntersectionResult BVHStaticIntersect (Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program);
}