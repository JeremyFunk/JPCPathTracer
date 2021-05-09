#pragma once
#include "Archive.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include <unordered_map>
#include <yaml-cpp/node/node.h>

namespace jpctracer::parse
{

// geometry

using GeometryPtr = std::shared_ptr<renderer::Geometry>;

bool LoadGeometry(const YAML::Node& node, GeometryPtr& geometry);

// materials

bool LoadMaterial(ShaderArchive& ar);

// renderer
bool LoadRenderer(const YAML::Node& node, JPCRenderer& renderer);

} // namespace jpctracer::parse