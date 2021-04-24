#pragma once
#include "jpc_tracer/core/core.h"
#include <memory.h>
#include <memory>
#include <utility>

namespace jpctracer::raytracing
{
/*
template<class T>
using Ref = std::shared_ptr<T>;

template<class T,class... ArgsT>
Ref<T> MakeRef(ArgsT&& ... args)
{
    return std::make_shared<T>(std::forward<ArgsT>(args)...);
}

template<class T>
using Scope = std::unique_ptr<T>;

template<class T,class... ArgsT>
Scope<T> MakeScope(ArgsT&& ... args)
{
    return std::make_unique<T>(std::forward<ArgsT>(args)...);
}



using Prec = float;
using Vec4 = Eigen::Vector4f;
using Vec3 = Eigen::Vector3f;
using Vec2 = Eigen::Vector2f;
using Vec2i = Eigen::Vector2i;
using Vec3i = Eigen::Vector3i;

using Mat3x3 = Eigen::Matrix<Prec, 3,3>;
using Mat4x4 = Eigen::Matrix<Prec, 4,4>;
*/

// Forward Decalaration

using AnyHitCallBack = const std::function<AnyHitResult(const SurfaceInteraction&)>&;

#ifdef _MSC_VER
#define M_E 2.71828182845904523536        // e
#define M_LOG2E 1.44269504088896340736    // log2(e)
#define M_LOG10E 0.434294481903251827651  // log10(e)
#define M_LN2 0.693147180559945309417     // ln(2)
#define M_LN10 2.30258509299404568402     // ln(10)
#define M_PI 3.14159265358979323846       // pi
#define M_PI_2 1.57079632679489661923     // pi/2
#define M_PI_4 0.785398163397448309616    // pi/4
#define M_1_PI 0.318309886183790671538    // 1/pi
#define M_2_PI 0.636619772367581343076    // 2/pi
#define M_2_SQRTPI 1.12837916709551257390 // 2/sqrt(pi)
#define M_SQRT2 1.41421356237309504880    // sqrt(2)
#define M_SQRT1_2 0.707106781186547524401 // 1/sqrt(2)
#endif

#define ERROR_THICCNESS 0.00001
#define SAMPLE_COUNT 8

} // namespace jpctracer::raytracing
