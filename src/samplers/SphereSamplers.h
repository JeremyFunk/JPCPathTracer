#pragma once
#include "core/Linalg.h"
namespace jpc_tracer
{
    Vec3 UniformSampleHemisphere(const Vec2& random_point);
    Vec2 ConcentricSampleDisk(const Vec2& random_point);
    Vec3 CosinusSampleHemisphere(const Vec2& random_point);
}