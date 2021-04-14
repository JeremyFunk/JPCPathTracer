#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/renderer/Geomtry.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

namespace jpctracer
{

    

    std::shared_ptr<renderer::Geometry> LoadMesh(std::string path);
    std::shared_ptr<renderer::Geometry> CreateTriangle(Vec3 vertex1,Vec3 vertex2, Vec3 vertex3);
}