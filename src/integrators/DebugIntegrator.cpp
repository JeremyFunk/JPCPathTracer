#include "DebugIntegrator.h"
namespace integrators {

    void DebugIntegrator::Init(std::shared_ptr<core::IScene> scene)
    {
        _scene = scene;
    }
    core::SpectrumPasses DebugIntegrator::Integrate(const core::Ray& ray,const std::shared_ptr<core::ISampler>& sampler,core::MemoryArea& memory) const
    {
        std::optional<core::SurfaceProperties> surfaceInteraction = _scene->Intersect(ray);
    
        core::SpectrumPasses luminance;
        if(surfaceInteraction.has_value()){
            
            core::Vec3 rgb = PixelEffect(surfaceInteraction.value(),ray,sampler,memory,_scene);
            luminance = core::SpectrumPasses(core::Spectrum::FromRGB(rgb));
        }
        return luminance;
    }
}