#include "BoundingBoxIntersection.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <cmath>
#include <corecrt_math.h>
#include <optional>

namespace jpctracer::raytracing
{
bool BoundsIntersect(const Bounds3D& bound, const Ray& ray, const Vec3& inverse_direction, const Int3& dir_is_negative)
{
    Prec x_min = (bound[dir_is_negative[0]][0] - ray.Origin[0]) * inverse_direction[0];
    Prec x_max = (bound[1 - dir_is_negative[0]][0] - ray.Origin[0]) * inverse_direction[0];

    Prec y_min = (bound[dir_is_negative[1]][1] - ray.Origin[1]) * inverse_direction[1];
    Prec y_max = (bound[1 - dir_is_negative[1]][1] - ray.Origin[1]) * inverse_direction[1];

    Prec z_min = (bound[dir_is_negative[2]][2] - ray.Origin[2]) * inverse_direction[2];
    Prec z_max = (bound[1 - dir_is_negative[2]][2] - ray.Origin[2]) * inverse_direction[2];

    Prec min_distance = std::max({x_min, y_min, z_min});

    Prec max_distance = std::min({x_max, y_max, z_max});

    if (min_distance > max_distance || max_distance <= 0 || ray.ClipEnd <= min_distance)
        return false;

    return true;
}
} // namespace jpctracer::raytracing