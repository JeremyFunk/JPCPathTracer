#pragma once

#include "jpc_tracer/core/maths/Transformation.h"
#include <vcruntime.h>
#include <algorithm>
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"

namespace jpctracer::raytracing {
    
    inline Vec3 ApplyTransformation(const Vec3& vec, const Transformation& trans)
    {
        math::Vec<Prec,4> result = trans.dot(math::Vec<Prec, 4>{vec[0], vec[1], vec[2], 1});
        return (result[3]==1) ? Vec3{result[0], result[1], result[2]} : Vec3{result[0]/result[3], result[1]/result[3], result[2]/result[3]};

        Prec x_trans = trans[0]*vec[0] + trans[1]*vec[1] + trans[2]*vec[2] + trans[3];
        Prec y_trans = trans[4]*vec[0] + trans[5]*vec[1] + trans[6]*vec[2] + trans[7];
        Prec z_trans = trans[8]*vec[0] + trans[9]*vec[1] + trans[10]*vec[2] + trans[11];
        Prec weights = trans[12]*vec[0] + trans[13]*vec[1] + trans[14]*vec[2] + trans[15];

        return (weights==1) ? Vec3{x_trans, y_trans, z_trans} : Vec3{x_trans/weights, y_trans/weights, z_trans/weights};
    }

    inline void ApplyTransformation(Vec3& vec, const Transformation& trans)
    {
        math::Vec<Prec,4> result = trans.dot(math::Vec<Prec, 4>{vec[0], vec[1], vec[2], 1});
        (result[3]==1) ? vec = {result[0], result[1], result[2]} : vec = Vec3{result[0]/result[3], result[1]/result[3], result[2]/result[3]};

        Prec x_trans = trans[0]*vec[0] + trans[1]*vec[1] + trans[2]*vec[2] + trans[3];
        Prec y_trans = trans[4]*vec[0] + trans[5]*vec[1] + trans[6]*vec[2] + trans[7];
        Prec z_trans = trans[8]*vec[0] + trans[9]*vec[1] + trans[10]*vec[2] + trans[11];
        Prec weights = trans[12]*vec[0] + trans[13]*vec[1] + trans[14]*vec[2] + trans[15];

        (weights==1) ? vec = {x_trans, y_trans, z_trans} : vec = Vec3{x_trans/weights, y_trans/weights, z_trans/weights};
    }

    inline Ray ApplyTransformation(const Ray& ray, const Transformation& trans)
    {
        return {ApplyTransformation(ray.Direction, trans), ApplyTransformation(ray.Origin, trans), ray.LensPosition, ray.ClipEnd, ray.Time};
    }

    // inline void ApplyTransformation(Ray& ray, const Transformation& trans)
    // {
    //     ApplyTransformation(ray.Direction, trans);
    //     ApplyTransformation(ray.Origin, trans);
    // }

    inline Bounds3D ApplyTransformation(const Bounds3D& bounds, const Transformation& trans)
    {
        const auto vec1 = ApplyTransformation(bounds.Min, trans);
        const auto vec2 = ApplyTransformation(bounds.Max, trans);

        const Vec3 min = {std::min(vec1[0], vec2[0]),
                          std::min(vec1[1], vec2[1]),
                          std::min(vec1[2], vec2[2])};

        const Vec3 max = {std::max(vec1[0], vec2[0]),
                          std::max(vec1[1], vec2[1]),
                          std::max(vec1[2], vec2[2])};

        return Bounds3D(min, max);
    }

    // inline void ApplyTransformation(Bounds3D& bounds, const Transformation& trans)
    // {
    //     ApplyTransformation(bounds.Min, trans);
    //     ApplyTransformation(bounds.Max, trans);

    //     for(int i = 0; i < 3; i++)
    //         if(bounds.Min[i] == bounds.Max[i]) std::swap(bounds.Min[i], bounds.Max[i]);
    // }
}