#include "SphericalCoordinates.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include <algorithm>



namespace bsdfs
{
    core::Vec3 SphericalDirection(core::Prec sin_theta,core::Prec cos_theta, core::Prec phi) 
    {
        return core::Vec3({sin_theta*std::cos(phi),sin_theta*std::sin(phi),cos_theta});
    }
    
    core::Vec3 SphericalDirection(core::Prec theta, core::Prec phi) 
    {
        return SphericalDirection(std::sin(theta),std::cos(theta),phi);
    }
    
    core::Prec SphericalTheta(const core::Vec3& vec) 
    {
        return std::acos(std::clamp<core::Prec>(vec[2],-1,1));
    }
    
    core::Prec SphericalPhi(const core::Vec3& vec) 
    {
        core::Prec p = std::atan2(vec[1],vec[0]);
        return (p<0) ? (p+2*M_PI) : M_PI;
    }
}