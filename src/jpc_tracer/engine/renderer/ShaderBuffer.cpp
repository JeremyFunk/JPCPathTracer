#include "ShaderBuffer.h"
#include <memory>
#include <optional>



namespace jpctracer::renderer
{
    uint ShaderBuffer::AddShader(std::unique_ptr<shadersys::IShader> &&shader) 
    {
        m_shaders.emplace_back(std::move(shader));
        return m_shaders.size()-1;
    }
    
    const shadersys::IShader* ShaderBuffer::GetShader(uint id) const
    {
        return m_shaders[id].get();
    }
    
    std::shared_ptr<MaterialSettings> MaterialLibrary::Get(std::string name) 
    {
        uint id;
        if(! Exists(name))
            id = 0;
        else
            id = m_name_id_map[name];

        auto material = std::make_shared<MaterialSettings>(m_default_settings[id]);
        m_created_materials.push_back(material);
        return material;

    }
    
    bool MaterialLibrary::Exists(std::string name) 
    {
        return m_name_id_map.contains(name);
    }
    
    ShaderBuffer MaterialLibrary::CreateShaders() 
    {
        ShaderBuffer buffer;
        for(const auto& material_settings: m_created_materials)
        {
            uint id = material_settings->material_id;
            buffer.AddShader(m_shader_builders[id](*material_settings));
        }
        return buffer;
    }
    
    
}