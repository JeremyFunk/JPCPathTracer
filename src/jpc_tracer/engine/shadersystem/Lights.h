#pragma once
#include "jpc_tracer/core/core.h"
#include "ShaderFunction.h"
#include "jpc_tracer/engine/shadersystem/lights/PointLights.h"
#include <vector>

namespace jpctracer::shadersys {

    class Lights
    {
    public:
        void AddPointLight(const Vec3& position,ShaderFunction<BsdfNode*> auto shader);
    
        PointLights point_lights;
    };

    void Lights::AddPointLight(const Vec3& position,ShaderFunction<BsdfNode*> auto shader) 
    {
        point_lights.Add(position, shader);        
    }

    
    
}