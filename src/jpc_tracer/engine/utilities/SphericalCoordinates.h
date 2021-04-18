#pragma once
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include <algorithm>

namespace jpctracer
{
    //Returns normalized Vector
    inline Norm3 SphericalDirection(Prec sin_theta,Prec cos_theta, Prec phi )
    {
        return Norm3::UnsafeConversion({sin_theta*std::cos(phi),sin_theta*std::sin(phi),cos_theta});
    }
    inline Norm3 SphericalDirection(Prec theta, Prec phi)
    {
        return SphericalDirection(std::sin(theta),std::cos(theta),phi);
    }

    inline Prec SphericalTheta(const Norm3& vec)
    {
        return std::acos(std::clamp<Prec>(vec[2],-1,1));
    }
    inline Prec SphericalPhi(const Norm3& vec)
    {
        Prec p = std::atan2(vec[1],vec[0]);
        return (p<0) ? (p+2*M_PI) : M_PI;
    }
    
    inline Prec CosTheta(const Norm3 &v) 
    { 
        return v[2]; 
    }

    inline Prec Cos2Theta(const Norm3 &v) 
    { 
        return v[2] * v[2]; 
    }

    inline Prec AbsCosTheta(const Norm3& v) 
    { 
        return std::abs(v[2]); 
    }

    inline Prec Sin2Theta(const Norm3& v) 
    {
        return std::max((Prec)0, (Prec)1 - Cos2Theta(v));
    }

    inline Prec SinTheta(const Norm3& v) 
    { 
        return std::sqrt(Sin2Theta(v)); 
    }

    inline Prec TanTheta(const Norm3& v) 
    { 
        return SinTheta(v) / CosTheta(v); 
    }

    inline Prec Tan2Theta(const Norm3 &v) 
    {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    inline Prec Tan2Theta(const Prec& cos2_theta) 
    {
        Prec sin2_theta = 1 -cos2_theta;
        Prec tan2_theta = sin2_theta / cos2_theta;
        return tan2_theta;
    }


    inline Prec CosPhi(const Norm3& v) 
    {
        Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 1 : std::clamp<Prec>(v[0] / sinTheta, -1, 1);
    }

    inline Prec SinPhi(const Norm3& v) 
    {
        Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 0 : std::clamp<Prec>(v[1] / sinTheta, -1, 1);
    }

    inline Prec Cos2Phi(const Norm3& v) 
    { 
        return CosPhi(v) * CosPhi(v); 
    }

    inline Prec Sin2Phi(const Norm3& v) 
    { 
        return SinPhi(v) * SinPhi(v); 
    }

    inline bool SameHemiSphereNormalSpace(const Vec3& v1, const Vec3& v2)
    {
        return v1[2]*v2[2]>=0;
    }



    inline Vec3 Reflect(const Vec3& v1,const Norm3& normal)
    {
        return  -1 * v1 + 2 * normal.dot(v1) * Vec3(normal);
    }

}