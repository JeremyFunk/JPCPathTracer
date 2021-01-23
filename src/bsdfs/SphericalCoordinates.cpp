#include "SphericalCoordinates.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include <algorithm>



namespace jpc_tracer
{
    Vec3 SphericalDirection(Prec sin_theta,Prec cos_theta, Prec phi) 
    {
        return Vec3({sin_theta*std::cos(phi),sin_theta*std::sin(phi),cos_theta});
    }
    
    Vec3 SphericalDirection(Prec theta, Prec phi) 
    {
        return SphericalDirection(std::sin(theta),std::cos(theta),phi);
    }
    
    Prec SphericalTheta(const Vec3& vec) 
    {
        return std::acos(std::clamp<Prec>(vec[2],-1,1));
    }
    
    Prec SphericalPhi(const Vec3& vec) 
    {
        Prec p = std::atan2(vec[1],vec[0]);
        return (p<0) ? (p+2*M_PI) : M_PI;
    }
}