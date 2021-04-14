#pragma once

#include <unordered_map>
#include <variant>
#include "../raytracing/Geometry.h"
#include "jpc_tracer/core/IAnimated.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/renderer/MaterialSettings.h"
namespace jpctracer::renderer
{
    class Geometry
    {
    public:
        std::variant<raytracing::TriangleMesh,raytracing::SphereMesh,raytracing::MeshId> mesh;
        std::shared_ptr<IAnimated<Transformation>> transformation = nullptr;

        std::unordered_map<uint, std::shared_ptr<MaterialSettings>> MaterialSlots;

        
    };
 }