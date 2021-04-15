#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"

namespace jpctracer::raytracing
{
    std::vector<Bounds3D> GenerateTriangleBounds(const TriangleMesh& mesh);
    std::vector<Vec3> GetTriangleCenters(const TriangleMesh& mesh);

    std::vector<Bounds3D> GenerateSphereBounds(const SphereMesh& mesh);
    std::vector<Vec3> GetSphereCenters(const SphereMesh& mesh);

    Vec3 GetBoxCenter(const Bounds3D& bound);
}