#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include <optional>

namespace jpctracer::raytracing
{
std::optional<SurfaceInteraction> Intersect(const SphereMesh& mesh, int idx, const Ray& ray,
                                            const int* materials_per_slot);
size_t GetSize(const SphereMesh& mesh);
} // namespace jpctracer::raytracing