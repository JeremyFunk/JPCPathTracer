#pragma once
#include "core/Linalg.h"
namespace bsdfs {
    core::Vec3 MicrofacetNormal(const core::Vec3& scattered_direction,const core::Vec3& incident_direction);
}