#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <algorithm>
#include <optional>

namespace jpctracer::raytracing
{
constexpr inline bool BoundsIntersect(const Bounds3D& bound, const Ray& ray, const Vec3& inverse_direction,
                                      const Int3& dir_is_negative)
{
    Prec x_min = (bound.Min[0] - ray.Origin[0]) * inverse_direction[0];
    Prec x_max = (bound.Max[0] - ray.Origin[0]) * inverse_direction[0];

    Prec y_min = (bound.Min[1] - ray.Origin[1]) * inverse_direction[1];
    Prec y_max = (bound.Max[1] - ray.Origin[1]) * inverse_direction[1];

    Prec z_min = (bound.Min[2] - ray.Origin[2]) * inverse_direction[2];
    Prec z_max = (bound.Max[2] - ray.Origin[2]) * inverse_direction[2];

    if (dir_is_negative[0])
        std::swap(x_min, x_max);
    if (dir_is_negative[1])
        std::swap(y_min, y_max);
    if (dir_is_negative[2])
        std::swap(z_min, z_max);

    Prec min_distance = std::max({x_min, y_min, z_min});

    Prec max_distance = std::min({x_max, y_max, z_max});
    return min_distance <= max_distance && max_distance > 0 && ray.ClipEnd > min_distance;
}
} // namespace jpctracer::raytracing