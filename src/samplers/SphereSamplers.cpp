#include "SphereSamplers.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include <cmath>
#include "core/constants.h"

namespace samplers
{
    core::Vec3 UniformSampleHemisphere(const core::Vec2& random_point) 
    {
        core::Prec z = sin(random_point[0]);

        core::Prec r = std::sqrt(std::max<core::Prec>(0,1-z*z));
        core::Prec phi = 2 * M_PI*random_point[1];
        return core::Vec3({r*std::cos(phi),r*std::sin(phi),z});
    }
}
