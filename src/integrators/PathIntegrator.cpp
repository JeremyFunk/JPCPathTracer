#include "PathIntegrator.h"
#include "core/IBSDF.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceInteraction.h"
#include "core/constants.h"
#include <corecrt_math.h>
#include<iostream>
#include <vector>

namespace integrators
{
    PathIntegrator::PathIntegrator(int sub_sample_count, int max_depth) 
        : _sub_sample_count(sub_sample_count), _max_depth(max_depth)
    {
        
    }



    void PathIntegrator::Init(std::shared_ptr<IScene> scene){
        _scene = scene;
    }
    SpectrumPasses PathIntegrator::Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const{
        if(ray.Depth>=_max_depth)
            return Spectrum::FromRGB({ 1,1,1 });
        
        
        auto surfaceInteraction = _scene->Intersect(ray);


        if(surfaceInteraction.has_value()){
            auto savepoint = memory.GetSavePoint();
            SurfaceInteraction& interaction = surfaceInteraction->Interaction;
            interaction.Point += surfaceInteraction->Interaction.Normal * ERROR_THICCNESS * 2;
            //auto minDist = std::max((Prec).1, surfaceInteraction->Distance * (Prec).05);
            //return SpectrumPasses(RGBSpectrum::FromRGB({minDist,minDist,minDist}));
            SpectrumPasses lights_spec =surfaceInteraction->Material->Illumination(interaction, ray);
            IBSDF* bsdf = surfaceInteraction->Material->ComputeBSDF(interaction, memory);
            Prec weights = 0;
            
            Ray outgoing_ray;
            outgoing_ray.Origin = interaction.Point;
            outgoing_ray.LensPosition = ray.LensPosition;
            outgoing_ray.Depth = ray.Depth+1;

            SpectrumPasses integral;

            SpectrumPasses integrated_lights = IntegrateAllLights(interaction);
            SpectrumPasses lights_color;
            //if (outgoing_ray.Depth == 2)
            //    std::cout << "Debug";
            
            auto samples2d = sampler->Get2DSampleArray(ceil(sqrt(_sub_sample_count)), ceil(sqrt(_sub_sample_count)));
            for(int sub_sample_idx = 0; sub_sample_idx<_sub_sample_count;sub_sample_idx++ )
            {
                Vec2 sample2d = samples2d->operator[](sub_sample_idx);
                //Vec2 sample2d = sampler->Get2DSample();

                DirectionInfo bsdf_info = bsdf->Evaluate(ray.Direction, sample2d);
                weights+=bsdf_info.Pdf;

                
                outgoing_ray.Direction = bsdf_info.OutgoingDirection;

                SpectrumPasses sub_rays_color = Integrate(outgoing_ray, sampler, memory);
                
                if(bsdf_info.Pdf<=ERROR_THICCNESS)
                    integral += sub_rays_color *bsdf_info.SurfaceColor;
                else
                    integral += (sub_rays_color / bsdf_info.Pdf)*bsdf_info.SurfaceColor;
                lights_color += integrated_lights * bsdf_info.SurfaceColor;
                //return Spectrum::FromValue(bsdf_info.Pdf);
                // if (ray.Depth == 0)
                // {
                //     //Direction of ray
                
                //     Vec3 dir = outgoing_ray.Direction;
                //     dir.normalize();
                //     Vec3 dir_color = (dir * (core::Prec)0.5) + Vec3(0.5, 0.5, 0.5);
                //     return Spectrum::FromRGB(dir_color);
                //     //return SpectrumPasses::FromValue(bsdf_info.Pdf);
                //     //return (sub_rays_color);
                //     //return (integral + result_spec) / weights;
                //     //return bsdf_info.SurfaceColor;
                // }
                // else {
                //     return sub_rays_color;
                // }
            }
            delete bsdf;
            
            

            memory.SetSavePoint(savepoint);
            /// (core::Prec)_sub_sample_count
            if(weights <= ERROR_THICCNESS)
                return lights_color / _sub_sample_count + integral / _sub_sample_count + lights_spec;
            else
                return lights_color / _sub_sample_count + integral / _sub_sample_count + lights_spec;
            //return Spectrum::FromRGB({ 1,0,1 });
            //
             
        }
        else
        {
            return Spectrum::FromRGB({ .1,0,0 });
        }
        

        //return SpectrumPasses(Spectrum::FromRGB({1,0,0}));
        return Spectrum::FromRGB({ 1,1,0 });
    }
    SpectrumPasses PathIntegrator::IntegrateAllLights(const SurfaceInteraction& interaction) const{
        Spectrum spectrum = Spectrum();
        //Vec3 normalColor = (interaction.Normal * (Prec)0.5) + Vec3(0.5, 0.5, 0.5);


        //return SpectrumPasses(Spectrum::FromValue(0));
        for(auto l : _scene->GetLights()){
            auto info = l->GetLightInformation(interaction.Point);

            //spectrum += RGBSpectrum::FromRGB((-info.Direction) * (Prec).5 + Vec3(.5,.5,.5));
            //continue;
            auto intersectionTest = _scene->Intersect(Ray(interaction.Point, -info.Direction));
            auto dot = std::max((Prec)0, (interaction.Normal).dot(-info.Direction));

            // auto minDist = -(interaction.Point);
            // spectrum = RGBSpectrum::FromRGB({minDist[0],minDist[1],minDist[2]});
            // continue;
            // spectrum += RGBSpectrum::FromRGB({dot,dot,dot}) ;
            //Vec3 normalColor = (interaction.Normal * (Prec)0.5) + Vec3(0.5, 0.5, 0.5);

            //spectrum += RGBSpectrum::FromRGB(normalColor);
            if(!intersectionTest.has_value() || intersectionTest->Interaction.Distance > info.Distance){
                spectrum += l->Illumination(interaction.Point, info);
                
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