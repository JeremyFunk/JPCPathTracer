#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/shadersystem/ShaderFunction.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include <vector>
namespace jpctracer::shadersys
{
    class PointLights
    {
    public:
        
        void Add(const Vec3& position, ShaderFunction<BsdfNode*> auto shader);
        //const RootShader* Sample(Ray* out_incident_ray,const SurfaceInteraction& interaction, Vec2 random_point) const;
    private:
        

        std::vector<Vec3> m_positions;

    };

    void PointLights::Add(const Vec3& position, ShaderFunction<BsdfNode*> auto shader)
    {
        SurfaceInteraction interaction = {position,Vec3{0},Vec2{0},0,0}; 
        NormalSpace space = {interaction, Vec3{0},Vec3{0},Vec3{0},Vec3{0}};
        //m_shaders.push_back(CreateShader<MATERIAL_EMISSION>(shader, &m_pool,space));
        m_positions.push_back(position);
    }
}