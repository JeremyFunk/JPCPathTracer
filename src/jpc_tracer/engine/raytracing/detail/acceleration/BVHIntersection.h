#pragma once

#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include <optional>

namespace jpctracer::raytracing
{
IntersectionResult BVHIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program);
} // namespace jpctracer::raytracing