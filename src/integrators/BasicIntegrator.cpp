#include "BasicIntegrator.h"


namespace integrators
{


    void BasicIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }
    SpectrumPasses BasicIntegrator::Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const{
        auto spectrum = RGBSpectrum::FromRGB(Vec3(ray.Direction[0]*.5+.5, ray.Direction[1]*.5+.5, ray.Direction[2]*.5+.5));
        return SpectrumPasses(spectrum);
    }
    SpectrumPasses BasicIntegrator::IntegrateAllLights(const SurfaceInteraction& interaction) const{
        return SpectrumPasses();
    }
}