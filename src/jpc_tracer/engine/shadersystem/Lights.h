#pragma once
#include "jpc_tracer/core/core.h"
#include "ShaderFunction.h"
#include "jpc_tracer/engine/shadersystem/BsdfStack.h"
#include "jpc_tracer/engine/shadersystem/RootShader.h"
#include "jpc_tracer/engine/shadersystem/lights/PointLights.h"
#include <vector>

namespace jpctracer::shadersys {

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