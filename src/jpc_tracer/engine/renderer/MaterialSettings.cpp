#include "MaterialSettings.h"
#include "jpc_tracer/core/maths/Spectrum.h"



namespace jpctracer::renderer
{
    MaterialSettings::MaterialSettings(std::string name, uint material_id) 
        : name(name), material_id(material_id)
    {
        
    }
    
    MaterialSettings::MaterialSettings() 
        : name("uninitialized"), material_id(0)
    {
        
    }
    
    Spectrum MaterialSettings::GetColor(std::string property) 
    {
        if(m_values.contains(property))
        {
            const auto&  prop = m_values[property];
            if(prop.index()==0)
                return std::get<Spectrum>(prop);
        }
            
        return FromRGB({0.5,0.5,0.5});
    }
    
    Prec MaterialSettings::GetValue(std::string property) 
    {
        if(m_values.contains(property))
        {
            const auto&  prop = m_values[property];
            if(prop.index()==1)
                return std::get<Prec>(prop);
        }
            
        return 0.5;
    }
    
    Vec3 MaterialSettings::GetNormal(std::string property) 
    {
        if(m_values.contains(property))
        {
            const auto&  prop = m_values[property];
            if(prop.index()==2)
                return std::get<Vec3>(prop);
        }
            
        return {0.5,0.5,0.5};
    }

    void MaterialSettings::SetColor(std::string property,Spectrum color) 
    {
        m_values[property] = color;
    }
    
    void MaterialSettings::SetValue(std::string property,Prec value) 
    {
        m_values[property] = value;
    }
    
    void MaterialSettings::SetNormal(std::string property,Vec3 normal) 
    {
        m_values[property] = normal;
    }
    
    void MaterialSettings::SetTexture(std::string property,std::string filepath) 
    {
        m_values[property] = filepath;
    }

    using properties_t = std::variant<Spectrum,Prec,Vec3,std::string>;
    using properties_value_pair = std::pair<std::string,properties_t>;
    using settings = std::initializer_list<properties_value_pair>;

    void MaterialSettings::SetMultiple(const std::initializer_list<properties_value_pair>& properties_with_value) 
    {
        for(const auto& pair : properties_with_value)
        {
            m_values[pair.first]=pair.second;
        }
    }
}