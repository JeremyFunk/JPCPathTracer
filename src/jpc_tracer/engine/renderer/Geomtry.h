#pragma once

#include "../raytracing/Geometry.h"
#include "jpc_tracer/core/IAnimated.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/shadersystem/Shader.h"
#include <unordered_map>
#include <variant>
namespace jpctracer::renderer
{
class Geometry
{
  public:
    Geometry(raytracing::TriangleMesh _mesh) : mesh(_mesh)
    {
    }
    Geometry(raytracing::SphereMesh _mesh) : mesh(_mesh)
    {
    }

    std::variant<raytracing::TriangleMesh, raytracing::SphereMesh, raytracing::MeshId> mesh;
    Transformation transformation = Transformation::Identity();

    std::unordered_map<uint, shadersys::IShaderRef> MaterialSlots;
};
} // namespace jpctracer::renderer