#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <optional>


namespace jpctracer::raytracing
{
    bool BoundsIntersect(const Bounds3D& bound, const Ray& ray, const Vec3& inverse_direction, const Int3& dir_is_negative);
}