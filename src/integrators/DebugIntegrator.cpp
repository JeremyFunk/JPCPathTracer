#include "DebugIntegrator.h"
#include "core/Linalg.h"
namespace integrators {

    void DebugIntegrator::Init(std::shared_ptr<core::IScene> scene)
    {
        _scene = scene;
    }
    
    size_t DebugIntegrator::Get2DSampleCount() 
    {
        return 1;
    }
    core::SpectrumPasses DebugIntegrator::Integrate(const core::Ray& ray,const core::Vec2* samples,core::MemoryArea& memory) const
    {
        std::optional<core::SurfaceProperties> surfaceInteraction = _scene->Intersect(ray);
    
        core::SpectrumPasses luminance;
        if(surfaceInteraction.has_value()){
            
            core::Vec3 rgb = PixelEffect(surfaceInteraction.value(),ray,*samples,memory,_scene);
            luminance = core::SpectrumPasses(core::Spectrum::FromRGB(rgb));
        }
        return luminance;
    }
}