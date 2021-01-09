#pragma once

#include "core/Linalg.h"
namespace bsdfs
{
    //Returns normalized Vector
    core::Vec3 SphericalDirection(core::Prec sin_theta,core::Prec cos_theta, core::Prec phi );
    core::Vec3 SphericalDirection(core::Prec theta, core::Prec phi);

    core::Prec SphericalTheta(const core::Vec3& vec);
    core::Prec SphericalPhi(const core::Vec3& vec);
    
    inline core::Prec CosTheta(const core::Vec3 &v) { return v[2]; }
    inline core::Prec Cos2Theta(const core::Vec3 &v) { return v[2] * v[2]; }
    inline core::Prec AbsCosTheta(const core::Vec3& v) { return std::abs(v[2]); }
    inline core::Prec Sin2Theta(const core::Vec3& v) {
        return std::max((core::Prec)0, (core::Prec)1 - Cos2Theta(v));
    }

    inline core::Prec SinTheta(const core::Vec3& v) { return std::sqrt(Sin2Theta(v)); }

    inline core::Prec TanTheta(const core::Vec3& v) { return SinTheta(v) / CosTheta(v); }

    inline core::Prec Tan2Theta(const core::Vec3 &v) {
        return Sin2Theta(v) / Cos2Theta(v);
    }

    inline core::Prec CosPhi(const core::Vec3& v) {
        core::Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 1 : std::clamp<core::Prec>(v[0] / sinTheta, -1, 1);
    }

    inline core::Prec SinPhi(const core::Vec3& v) {
        core::Prec sinTheta = SinTheta(v);
        return (sinTheta == 0) ? 0 : std::clamp<core::Prec>(v[1] / sinTheta, -1, 1);
    }

    inline core::Prec Cos2Phi(const core::Vec3& v) { return CosPhi(v) * CosPhi(v); }

    inline core::Prec Sin2Phi(const core::Vec3& v) { return SinPhi(v) * SinPhi(v); }

    inline bool SameHemiSphereNormalSpace(const core::Vec3& v1, const core::Vec3& v2)
    {
        return v1[2]*v2[2]>0;
    }

    inline core::Vec3 Reflect(const core::Vec3& v1,const core::Vec3& v2)
    {
        return  -v1 + 2 * v1.dot(v2) * v2;
    }
}