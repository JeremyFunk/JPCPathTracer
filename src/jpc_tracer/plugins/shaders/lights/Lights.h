#pragma once
#include "jpc_tracer/core/core.h"

namespace jpctracer {
    class Lights
    {
    public:
        template<class Iterator ,class MaterialBuilder>
        void AddLight(std::shared_ptr<IGeometryRange<Iterator>> geometry, MaterialBuilder builder);
    };
    
}