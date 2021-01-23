#pragma once

#include "core/Linalg.h"
namespace jpc_tracer
{
    //Returns normalized Vector
    Vec3 SphericalDirection(Prec sin_theta,Prec cos_theta, Prec phi );
    Vec3 SphericalDirection(Prec theta, Prec phi);

    Prec SphericalTheta(const Vec3& vec);
    Prec SphericalPhi(const Vec3& vec);
    
    inline Prec CosTheta(const Vec3 &v) { return v[2]; }
    inline Prec Cos2Theta(const Vec3 &v) { return v[2] * v[2]; }
    inline Prec AbsCosTheta(const Vec3& v) { return std::abs(v[2]); }
    inline Prec Sin2Theta(const Vec3& v) {
        return std::max((Prec)0, (Prec)1 - Cos2Theta(v));
    }

    inline Prec SinTheta(const Vec3& v) { return std::sqrt(Sin2Theta(v)); }

    inline Prec TanTheta(const Vec3& v) { return SinTheta(v) / CosTheta(v); }

    inline Prec Tan2Theta(const Vec3 &v) {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    inline Prec Tan2Theta(const Prec& cos2_theta) {
        Prec sin2_theta = 1 -cos2_theta;
        Prec tan2_theta = sin2_theta / cos2_theta;
        return tan2_theta;
    }


    inline Prec CosPhi(const Vec3& v) {
        Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 1 : std::clamp<Prec>(v[0] / sinTheta, -1, 1);
    }

    inline Prec SinPhi(const Vec3& v) {
        Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 0 : std::clamp<Prec>(v[1] / sinTheta, -1, 1);
    }

    inline Prec Cos2Phi(const Vec3& v) { return CosPhi(v) * CosPhi(v); }

    inline Prec Sin2Phi(const Vec3& v) { return SinPhi(v) * SinPhi(v); }

    inline bool SameHemiSphereNormalSpace(const Vec3& v1, const Vec3& v2)
    {
        return v1[2]*v2[2]>=0;
    }

    inline Vec3 Reflect(const Vec3& v1,const Vec3& normal)
    {
        return  -v1 + 2 * v1.dot(normal) * normal;
    }
}