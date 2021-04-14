#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <initializer_list>
#include <unordered_map>
#include <variant>

namespace jpctracer::renderer
{
    enum class MatInputs
    {
        Color,
        Value,
        Normal,
        Texture
    };
    class MaterialSettings
    {
    public:
        using properties_t = std::variant<Spectrum,Prec,Vec3,std::string>;
        using properties_value_pair = std::pair<std::string,properties_t>;
        using settings = std::initializer_list<properties_value_pair>;

        MaterialSettings(std::string name, uint material_id);
        MaterialSettings();

        Spectrum GetColor(std::string property);
        Prec GetValue(std::string property);
        Vec3 GetNormal(std::string property);
        

        void SetColor(std::string property,Spectrum color);
        void SetValue(std::string property,Prec value);
        void SetNormal(std::string property,Vec3 normal);
        void SetTexture(std::string property,std::string filepath);
        void SetMultiple(const settings& properties_with_value);

        const uint material_id;
        const std::string name;
        //Maby
        //bool ContainsProperty(std::string property);
        //bool ContainsProperty(std::string property, MatInputs input_type);
        //std::vector<std::pair<MatInputs,std::string>> AvailableProperties();
    private:
        
        std::unordered_map<std::string, properties_t> m_values;


    };
}