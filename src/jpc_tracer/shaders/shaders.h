#pragma once
#include "jpc_tracer/maths/maths.h"
#include <type_traits>
namespace jpctracer
{
    namespace shader
    {
        enum class MaterialType
        {
            DIFFUSE,
            GLOSSY,
            Transmission,
            EMISSION        
        };

    }

}