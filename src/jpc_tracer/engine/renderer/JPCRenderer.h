#pragma once
#include "ShaderBuffer.h"
#include "jpc_tracer/engine/films/Film.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/renderer/Geomtry.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/renderer/RenderInterfaces.h"
#include <memory>


namespace jpctracer::renderer
{
    class JPCRenderer
    {
    public:
        JPCRenderer(std::unique_ptr<ISampler>&& sampler, std::unique_ptr<ICamera>&& camera, std::unique_ptr<IIntegrator>&& integrator);
        MaterialLibrary MaterialLib;
        shadersys::Lights LightsLib;

        size_t TileSize = 16;
        raytracing::AccelerationSettings Acceleration;


        void Draw(std::shared_ptr<Geometry> geomtry);
        //void Draw(std::shared_ptr<Light> light);
        
        void Render(uint width,uint height,std::string directory);
    private:
        std::unique_ptr<ISampler> m_sampler;
        std::unique_ptr<ICamera> m_camera;
        std::unique_ptr<IIntegrator> m_integrator;
        raytracing::SceneBuilder m_scene_builder;

    };
}