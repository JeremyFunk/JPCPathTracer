#pragma once

#include <cstddef>
namespace jpctracer {
    enum MaterialType
    {
        
        MATERIAL_DIFFUSE = 1<<0,
        MATERIAL_GLOSSY = 1<<1,
        MATERIAL_TRANSMISSION = 1 << 2,
        MATERIAL_SUBSURFACE  = 1 << 3,
        MATERIAL_EMISSION = 1<<4,
        MATERIAL_TRANSPARENCY = 1 <<5,
        MATERIAL_BSDF = MATERIAL_DIFFUSE|MATERIAL_GLOSSY|MATERIAL_TRANSMISSION
                    |MATERIAL_SUBSURFACE
    };

    constexpr bool ContainsMaterialType(const int& type1,const int& type2)
    {
        return (type1 & type2) != 0;
    }

    constexpr size_t MaterialTypeToId(const MaterialType& type)
    {
        switch (type) {
            case MATERIAL_DIFFUSE: return 0;
            case MATERIAL_GLOSSY: return 1;
            case MATERIAL_TRANSMISSION: return 2;
            case MATERIAL_SUBSURFACE: return 3;
            case MATERIAL_EMISSION: return 4;
            case MATERIAL_TRANSPARENCY: return 5;
            case MATERIAL_BSDF: return 6;
            
        }
    }

}