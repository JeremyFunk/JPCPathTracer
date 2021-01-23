#pragma once
#include "shapes/TriangleMesh.h"
#include <iostream>
#include <fstream>
#include <string>

namespace jpc_tracer
{
    std::shared_ptr<TriangleMesh> LoadMesh(std::string path,std::shared_ptr<IMaterial> material, std::shared_ptr<Transformation> transformation, std::shared_ptr<std::vector<std::shared_ptr<IShape>>> shapes);
}