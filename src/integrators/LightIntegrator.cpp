#include "LightIntegrator.h"

namespace integrators
{


    void LightIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }
    SpectrumPasses LightIntegrator::Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const{
        auto surfaceInteraction = _scene->Intersect(ray);
        
        if(surfaceInteraction.has_value()){
            //return SpectrumPasses(Spectrum(RGBSpectrum::FromRGB({surfaceInteraction->Interaction.UV[0], surfaceInteraction->Interaction.UV[1], 0})));
            //return SpectrumPasses(RGBSpectrum::FromRGB({1,1,1}));
            surfaceInteraction->Interaction.Point += surfaceInteraction->Interaction.Normal * ERROR_THICCNESS;
            //auto minDist = std::max((Prec).1, surfaceInteraction->Distance * (Prec).05);
            //return SpectrumPasses(RGBSpectrum::FromRGB({minDist,minDist,minDist}));
            return IntegrateAllLights(surfaceInteraction->Interaction);
        }

        return SpectrumPasses();
    }
    SpectrumPasses LightIntegrator::IntegrateAllLights(const SurfaceInteraction& interaction) const{
        Spectrum spectrum = Spectrum();

        for(auto l : _scene->GetLights()){
            auto info = l->GetLightInformation(interaction.Point);

            //spectrum = RGBSpectrum::FromRGB({1,1,1});
            //continue;
            //spectrum += RGBSpectrum::FromRGB((-info.Direction) * (Prec).5 + Vec3(.5,.5,.5));
            //continue;
            auto intersectionTest = _scene->Intersect(Ray(interaction.Point, -info.Direction));
            auto dot = std::max((Prec)0, (interaction.Normal).dot(-info.Direction));

            // auto minDist = -(interaction.Point + Vec3(0,0,1)) * .2;
            // spectrum = RGBSpectrum::FromRGB({minDist[2],minDist[2],minDist[2]});
            // continue;
            // spectrum += RGBSpectrum::FromRGB({dot,dot,dot}) ;
            Vec3 normalColor = (interaction.Normal * (Prec)0.5) + Vec3(0.5, 0.5, 0.5);

            spectrum += RGBSpectrum::FromRGB(normalColor);
            if(!intersectionTest.has_value() || intersectionTest->Interaction.Distance > info.Distance){
                //spectrum += l->Illumination(interaction.Point, info) * dot;
                //auto minDist = (interaction.Distance - 3.8f) * .3f;
                //spectrum += RGBSpectrum::FromRGB({minDist, minDist, minDist});
                //spectrum = RGBSpectrum::FromRGB({1,1,1});
                //auto minDist = std::max((Prec).1, -(interaction.Point[2]));
                //spectrum = RGBSpectrum::FromRGB({minDist,minDist,minDist});
            }
        }
        
        return SpectrumPasses(spectrum);
    }
}