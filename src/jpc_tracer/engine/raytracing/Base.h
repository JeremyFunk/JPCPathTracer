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

#define ERROR_THICCNESS 0.00001
#define SAMPLE_COUNT 8

} // namespace jpctracer::raytracing
