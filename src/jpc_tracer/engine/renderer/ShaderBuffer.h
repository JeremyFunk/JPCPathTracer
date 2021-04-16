#pragma once
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/engine/shadersystem/BsdfNode.h"
#include "jpc_tracer/engine/shadersystem/IBsdfClosure.h"
#include <memory>
#include <vector>
#include"MaterialSettings.h"
#include"jpc_tracer/core/core.h"
#include"../shadersystem/ShaderFunction.h"


namespace jpctracer::renderer
{
    class ShaderBuffer
    {
    
    public:
        ShaderBuffer() = default;
        ShaderBuffer(const ShaderBuffer&) = delete;
        ShaderBuffer(ShaderBuffer&&) = default;
        uint AddShader(std::unique_ptr<shadersys::IShader> &&shader);
        const shadersys::IShader* GetShader(uint id) const;
    private:
        std::vector<std::unique_ptr<shadersys::IShader>> m_shaders = {};
    };

    template<class T>
    concept ShaderBuilder = requires(T builder, MaterialSettings settings)
    {
        {builder(settings)}->shadersys::ShaderFunction<shadersys::BsdfNode>;
    };
    
    
    class MaterialLibrary
    {
    public:
        void Register(std::string name, const ShaderBuilder auto& builder);
        void Register(std::string name, const ShaderBuilder auto& builder,const MaterialSettings::settings& default_settings);
        std::shared_ptr<MaterialSettings> Get(std::string name);
        bool Exists(std::string name);
        ShaderBuffer CreateShaders();
        
    private:

        std::unique_ptr<shadersys::IShader> CreateShader(MaterialSettings settings);
        std::vector<std::function<std::unique_ptr<shadersys::IShader>(MaterialSettings)>> m_shader_builders;
        std::vector<MaterialSettings> m_default_settings;
        std::unordered_map<std::string,uint> m_name_id_map;

        std::vector<std::shared_ptr<MaterialSettings>> m_created_materials;

        TextureBuffer m_texture_buffer;

    };
    void MaterialLibrary::Register(std::string name, const ShaderBuilder auto& builder) 
    {

        m_shader_builders.emplace_back([builder](MaterialSettings settings) -> std::unique_ptr<shadersys::IShader>
        {
            auto shader_func= builder(settings);
            std::unique_ptr<shadersys::IShader> shader = std::make_unique<shadersys::Shader<decltype(shader_func)>>(shader_func);
            return shader;
        });

        uint type_id = m_shader_builders.size()-1;

        m_default_settings.push_back(MaterialSettings(name,type_id,&m_texture_buffer));
        m_name_id_map[name] = type_id;
    }

    void MaterialLibrary::Register(std::string name, const ShaderBuilder auto& builder,const MaterialSettings::settings& default_settings) 
    {
        m_shader_builders.emplace_back([builder](MaterialSettings settings) -> std::unique_ptr<shadersys::IShader>
        {
            auto shader_func= builder(settings);
            std::unique_ptr<shadersys::IShader> shader = std::make_unique<shadersys::Shader<decltype(shader_func)>>(shader_func);
            return shader;
        });

        uint type_id = m_shader_builders.size()-1;

        MaterialSettings default_set(name,type_id,&m_texture_buffer);
        default_set.SetMultiple(default_settings);

        m_default_settings.push_back(default_set);
        m_name_id_map[name] = type_id;
    }
    
}