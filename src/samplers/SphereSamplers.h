#pragma once
#include "core/Linalg.h"
namespace samplers
{
    core::Vec3 UniformSampleHemisphere(const core::Vec2& random_point);
    core::Vec2 ConcentricSampleDisk(const core::Vec2& random_point);
    core::Vec3 CosinusSampleHemisphere(const core::Vec2& random_point);
}