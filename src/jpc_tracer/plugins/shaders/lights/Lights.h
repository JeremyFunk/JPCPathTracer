#pragma once
#include "jpc_tracer/core/core.h"
#include "../ShaderFunction.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfStack.h"
#include "jpc_tracer/plugins/shaders/bsdf/RootShader.h"
#include "jpc_tracer/plugins/shaders/lights/PointLights.h"
#include <vector>

namespace jpctracer {

    class Lights
    {
    public:
        void AddPointLight(const Vec3& position,ShaderFunction<BsdfNode*> auto shader);
    private:
        PointLights m_point_lights;
    };

    void Lights::AddPointLight(const Vec3& position,ShaderFunction<BsdfNode*> auto shader) 
    {
        m_point_lights.Add(position, shader);        
    }

    
    
}