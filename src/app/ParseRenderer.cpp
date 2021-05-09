#include "ParseScene.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include <memory>
namespace jpctracer::parse
{
using GeometryMap = std::unordered_map<std::string, GeometryPtr>;

bool LoadGeometries(const YAML::Node& root_n, GeometryMap& geometries)
{
    std::string mat_name;
    for (const auto& mat_pair : root_n)
    {
        if (!Load(mat_pair.first, mat_name))
            return false;
        GeometryPtr geometry;
        if (!LoadGeometry(mat_pair.second, geometry))
            return false;
        geometries[mat_name] = geometry;
    }
    return true;
}

using ShaderMap = std::unordered_map<std::string, IShaderRef>;

bool LoadMaterials(const YAML::Node& node, ShaderMap& map, renderer::MaterialLibrary& lib)
{
    int counter = 0;
    std::string name;

    for (const auto& mat_node : node)
    {
        if (!Load(mat_node.first, name))
        {
            JPC_LOG_ERROR("Could not load {}. Material", counter);
            return false;
        }
        ShaderArchive ar{mat_node.second, lib};

        if (!LoadMaterial(ar))
        {
            JPC_LOG_ERROR("Could not load Material {}", name);
            return false;
        }
        map[name] = ar.shader;
    }
    return true;
}

bool LoadInstance(const YAML::Node& node, ShaderMap& shaders, GeometryPtr& geometry)
{
    std::string property;
    Vec3 rotation = {0, 0, 0};
    Vec3 translation = {0, 0, 0};
    Vec3 scale3 = {1, 1, 1};
    Prec scalef = 0;

    bool any_success = false;

    for (const auto& prop_node : node)
    {
        bool inst_success;
        if (Load(prop_node.first, property))
        {
            const auto& val_node = prop_node.second;
            if (property == "rot")
                inst_success = Load(val_node, rotation);
            else if (property == "scl")
                inst_success = Load(val_node, scale3) || Load(val_node, scalef);
            else if (property == "trans")
                inst_success = Load(val_node, translation);
            else if (property.substr(0, 3) == "mat")
            {
                int material_idx = std::stoi(property.substr(3));
                if (material_idx < 0)
                {
                    JPC_LOG_ERROR("Material_idx {} < 1", material_idx);
                    return false;
                }
                std::string material_name;

                inst_success = Load(val_node, material_name);

                geometry->MaterialSlots[material_idx] = shaders[material_name];
            }
            if (!inst_success)
            {
                JPC_LOG_WARN("Could not load instance: \n ----------------------------------- {} \n "
                             "-----------------------------------",
                             val_node);
            }
        }
        any_success = any_success || inst_success;
    }
    if (scalef == 0)
        geometry->transformation = TransScalRot(translation, scale3, rotation);
    else
        geometry->transformation = TransScalRot(translation, scalef, rotation);

    return any_success;
}

bool LoadInstances(const YAML::Node& node, ShaderMap& shaders, GeometryMap& geometries, JPCRenderer& renderer)
{
    std::string geometry_name;

    for (const auto& instance_node : node)
    {
        if (!Load(instance_node.first, geometry_name))
        {
            JPC_LOG_ERROR("Could not load instance");
            return false;
        }
        if (!geometries.contains(geometry_name))
        {
            JPC_LOG_ERROR("Meshname {} is unkown.", geometry_name);
            return false;
        }
        if (!LoadInstance(instance_node.second, shaders, geometries[geometry_name]))
        {
            JPC_LOG_ERROR("Could not load instance \"{}\" with data: \n{}", geometry_name, instance_node.second);
            return false;
        }
        renderer.Draw(geometries[geometry_name]);
    }
    return true;
}
// clang-format off
bool LoadLights(const YAML::Node& node, shadersys::Lights& lights)
{
    std::string light_type;
    for (const auto& light_node : node)
    {
        if (!Load(light_node.first, light_type))
            return false;
        if (light_type == "PointLight")
        {
            Vec3 position = {0,5,0};
            Spectrum color = FromRGB({1,0,1});
            Prec intensity = 1;

            if (    Load(light_node.second["Pos"], position)
                &&  Load(light_node.second["Color"],color) 
                )
            {
                Load(light_node.second["Intensity"],intensity);
                lights.AddPointLight(position, color*intensity);
            }
            else
                return false;
        }
        else if (light_type == "Sun") {
            Vec3 direction;
            Spectrum color;
            Prec intensity = 1;

            if (    Load(light_node.second["Dir"], direction)
                ||  Load(light_node.second["Color"],color) 
                ||  Load(light_node.second["Intensity"],intensity))
                lights.AddDistanceLight(direction, color*intensity);
            else
                return false;
        }else
        {
            return false;
        }
    }
    return true;
}

bool LoadRendererSettings(const YAML::Node& node, JPCRenderer& renderer)
{
    int tile_size = 64;
    std::string static_acc = "LBVH";
    std::string dynamic_acc = "NAIVE";
    bool success = Load(node["should_multithread"],renderer.ShouldMultiThread) 
                || Load(node["tile_size"],tile_size)
                || Load(node["static_acceleration"],static_acc)
                || Load(node["dynamic_acceleration"],dynamic_acc);
    renderer.TileSize = tile_size;
    
    if(static_acc == "NAIVE")
        renderer.Acceleration.StaticBVH = raytracing::StaticBVHType::NAIVE;
    else if (static_acc == "LBVH")
        renderer.Acceleration.StaticBVH = raytracing::StaticBVHType::LBVH;
    else
    {
        JPC_LOG_ERROR("{} unkown static bvh type");
        return false;
    }
    return success;
}

// clang-format on

bool LoadRenderer(const YAML::Node& node, JPCRenderer& renderer)
{
    ShaderMap shaders;

    if (!LoadMaterials(node["Materials"], shaders, renderer.MaterialLib))
        return false;

    GeometryMap geometries;

    if (!LoadGeometries(node["Meshes"], geometries))
        return false;

    if (!LoadInstances(node["Instances"], shaders, geometries, renderer))
        return false;
    if (!LoadLights(node["Lights"], renderer.LightsLib))
        return false;
    if (!LoadRendererSettings(node["Renderer"], renderer))
        return false;
    return true;
}

} // namespace jpctracer::parse