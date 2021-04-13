#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include <iostream>
#include <fstream>
#include <string>

namespace jpctracer::utility
{

    

    raytracing::TriangleMesh LoadMesh(std::string path);
}