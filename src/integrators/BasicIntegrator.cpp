#include "BasicIntegrator.h"


namespace integrators
{


    void BasicIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }
    
    size_t BasicIntegrator::Get2DSampleCount() 
    {
        return 1;
    }
    SpectrumPasses BasicIntegrator::Integrate(const Ray& ray,const core::Vec2* samples,MemoryArea& memory) const{
        auto spectrum = RGBSpectrum::FromRGB(Vec3(ray.Direction[0]*.5+.5, ray.Direction[1]*.5+.5, ray.Direction[2]*.5+.5));
        return SpectrumPasses(spectrum);
    }

}