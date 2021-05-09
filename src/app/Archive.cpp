#include "Archive.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Vec.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include <string>
#include <yaml-cpp/exceptions.h>

namespace jpctracer::parse
{

template <class T> bool LoadGeneric(const YAML::Node& node, T& val)
{
    if (!node.IsDefined())
        return false;
    auto temp = val;
    try
    {
        val = node.as<T>();
    }
    catch (const YAML::BadConversion& e)
    {
        JPC_LOG_INFO("Could not convert: {}", node);
        val = temp;
        return false;
    }
    return true;
}

template <size_t _Size> bool LoadVec(const YAML::Node& node, math::Vec<Prec, _Size>& vec)
{
    if (!(node.IsDefined() && node.IsSequence()))
        return false;
    int i = 0;
    bool success = true;
    math::Vec<Prec, _Size> temp;
    for (const auto& sub_vec3 : node)
    {
        if (i >= _Size)
            return false;
        success = success && LoadGeneric(sub_vec3, temp[i]);
        i++;
    }
    if (i != _Size)
        return false;
    vec = temp;
    return success;
}

bool Load(const YAML::Node& node, std::string& str)
{
    return LoadGeneric(node, str);
}

bool Load(const YAML::Node& node, Spectrum& spec)
{
    if (!LoadVec(node, spec))
    {
        Vec3 rgb;
        if (!LoadVec(node, rgb))
            return false;
        spec = FromRGB(rgb);
    }
    return true;
}

bool Load(const YAML::Node& node, Vec3& vec)
{
    return LoadVec(node, vec);
}

bool Load(const YAML::Node& node, Norm3& norm)
{
    Vec3 vec;
    if (!LoadVec(node, vec))
        return false;
    norm = vec.normalized();
    return true;
}

bool Load(const YAML::Node& node, Prec& val)
{
    return LoadGeneric(node, val);
}

bool Load(const YAML::Node& node, bool& val)
{
    return LoadGeneric(node, val);
}

bool Load(const YAML::Node& node, int& val)
{
    return LoadGeneric(node, val);
}

bool Load(const YAML::Node& node, std::array<Vec3, 3>& arr)
{
    const auto& subn = node;
    if (!(subn.IsDefined() && subn.IsSequence()))
        return false;
    int i = 0;
    bool success = true;
    for (const auto& sub_vec3 : subn)
    {
        if (i >= 3)
            return false;
        success = success && LoadVec(sub_vec3, arr[i]);
        i++;
    }
    return success;
}

bool Load(const ShaderArchive& ar, std::string name, std::string& str)
{
    return Load(ar.node[name], str);
}
bool Load(const ShaderArchive& ar, std::string name, Spectrum& spec)
{
    auto& node = ar.node[name];
    if (!Load(node, spec))
    {
        std::string path;
        if (!Load(node, path))
            return false;
        return ar.TexBindS(ar.shader_data, spec, path);
    }

    return true;
}
bool Load(const ShaderArchive& ar, std::string name, Vec3& vec)
{
    auto& node = ar.node[name];
    if (!Load(node, vec))
    {
        std::string path;
        if (!Load(node, path))
            return false;
        return ar.TexBindV(ar.shader_data, vec, path);
    }

    return true;
}

bool Load(const ShaderArchive& ar, std::string name, Norm3& vec)
{
    auto& node = ar.node[name];
    if (!Load(node, vec))
    {
        std::string path;
        if (!Load(node, path))
            return false;
        return ar.TexBindN(ar.shader_data, vec, path);
    }

    return true;
}

bool Load(const ShaderArchive& ar, std::string name, Prec& val)
{
    auto& node = ar.node[name];
    if (!Load(node, val))
    {
        std::string path;
        if (!Load(node, path))
            return false;
        return ar.TexBindP(ar.shader_data, val, path);
    }

    return true;
}
} // namespace jpctracer::parse