#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"
#include <optional>


namespace jpctracer::raytracing
{
    bool BoundsIntersect(const Bounds3D& bound, const Ray& ray, const Vec3& inverse_direction, const int dir_is_negative[3]);
}