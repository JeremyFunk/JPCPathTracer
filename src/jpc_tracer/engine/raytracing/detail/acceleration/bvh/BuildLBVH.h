#pragma once

#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include <vector>

namespace jpctracer::raytracing
{
BVHTree BuildLBVH(std::vector<Bounds3D>&& bounds, std::vector<uint32_t>&& morton_codes);
}