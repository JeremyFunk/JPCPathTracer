#pragma once
#include "shapes/TriangleMesh.h"
#include <iostream>
#include <fstream>
#include <string>

namespace utilities
{
    std::shared_ptr<shapes::TriangleMesh> LoadMesh(std::string path,std::shared_ptr<core::IMaterial> material, std::shared_ptr<core::Transformation> transformation, std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> shapes);
}