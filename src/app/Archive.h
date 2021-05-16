#pragma once
#include "jpc_tracer/JPCTracer.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "yaml-cpp/yaml.h"
#include <memory>
#include <unordered_map>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/node.h>

namespace jpctracer::parse
{

bool Load(const YAML::Node& node, Spectrum& spec);
bool Load(const YAML::Node& node, Vec3& vec);
bool Load(const YAML::Node& node, Norm3& norm);
bool Load(const YAML::Node& node, Prec& val);
bool Load(const YAML::Node& node, bool& val);
bool Load(const YAML::Node& node, int& val);
bool Load(const YAML::Node& node, std::array<Vec3, 3>& arr);
bool Load(const YAML::Node& node, std::string& str);

struct ShaderArchive
{
    const YAML::Node& node;
    renderer::MaterialLibrary& matlib;
    IShaderRef shader;
    void* shader_data;
    bool (*TexBindS)(void*, Spectrum&, std::string);
    bool (*TexBindP)(void*, Prec&, std::string);
    bool (*TexBindV)(void*, Vec3&, std::string);
    bool (*TexBindN)(void*, Norm3&, std::string);
};

bool Load(const ShaderArchive& ar, std::string name, Spectrum& spec);
bool Load(const ShaderArchive& ar, std::string name, Vec3& vec);
bool Load(const ShaderArchive& ar, std::string name, Norm3& vec);
bool Load(const ShaderArchive& ar, std::string name, Prec& val);

template <class T>
bool LoadShader(ShaderArchive& ar, std::string shader_type, bool (*loader)(const ShaderArchive&, T&));

namespace detail
{
template <class ShT, class _ValT> bool BindTexture(void* _shader, _ValT& val, std::string name)
{
    ShT* shader = (ShT*)_shader;
    return shader->BindTexture(&val, name);
}

template <class T> void update_shaderarchive(ShaderArchive& ar, ShaderRef<T>& shaderref)
{
    ar.shader = shaderref;
    ar.shader_data = (void*)&shaderref;
    ar.TexBindS = BindTexture<ShaderRef<T>, Spectrum>;
    ar.TexBindP = BindTexture<ShaderRef<T>, Prec>;
    ar.TexBindV = BindTexture<ShaderRef<T>, Vec3>;
    ar.TexBindN = BindTexture<ShaderRef<T>, Norm3>;
}

} // namespace detail

template <class T> bool LoadShader(ShaderArchive& ar, std::string shader_type, bool (*loader)(ShaderArchive&, T&))
{
    std::string type;
    if (!Load(ar.node["type"], type))
        return false;
    if (type != shader_type)
        return false;
    ShaderRef<T> shader = ar.matlib.Create<T>();
    detail::update_shaderarchive(ar, shader);
    return loader(ar, shader.get());
}

} // namespace jpctracer::parse