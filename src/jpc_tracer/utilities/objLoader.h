#pragma once
#include "shapes/TriangleMesh.h"
#include <iostream>
#include <fstream>
#include <string>

namespace jpc_tracer
{
    Ref<TriangleMesh> LoadMesh(std::string path,Ref<IMaterial> material, Ref<Transformation> transformation, Ref<std::vector<Ref<IShape>>> shapes);
}