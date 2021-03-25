#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"
#include <optional>


namespace jpctracer::raytracing
{
    struct Bound;

    std::optional<IntersectionInfo> BoundsIntersect(const Bound& bound, const Ray& ray, const Vec3& inverse_direction, const int dir_is_negative[3]);
}