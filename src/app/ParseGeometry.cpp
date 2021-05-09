#include "ParseScene.h"
#include "app/Archive.h"

namespace jpctracer::parse
{

bool LoadObj(const YAML::Node& node, GeometryPtr& geometry)
{
    std::string path;
    bool success = Load(node["path"], path);
    if (success)
        geometry = LoadMesh(path);
    return success;
}

bool LoadTris(const YAML::Node& node, GeometryPtr& geometry)
{
    std::array<Vec3, 3> tris;
    bool success = Load(node["tris"], tris);
    if (success)
        geometry = CreateTriangle(tris[0], tris[1], tris[2]);
    return success;
}
// clang-format off
bool LoadSphere(const YAML::Node& node, GeometryPtr& geometry)
{
    Prec radius;
    Vec3 position;
    bool success = Load(node["radius"], radius) 
                && Load(node["position"], position);
    if(success)
        geometry = CreateSphere(position, radius);
    return success;
}

bool LoadGeometry(const YAML::Node& node, GeometryPtr& geometry)
{
    return LoadObj(node, geometry)
        || LoadSphere(node, geometry)
        || LoadTris(node, geometry);
}
// clang-format on

} // namespace jpctracer::parse