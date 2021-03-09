#pragma once

#include "jpc_tracer/Concepts.h"
#include "jpc_tracer/maths/maths.h"

namespace jpctracer {
    namespace shader {
        template<class T>
        bool IsDeltaDistribution(T t)
        {
            return t.IsDeltaDistribution();
        }
        template<class T>
        concept DistributionFunction = requires(const T& func,Ray incident_ray,Vec2 random_p2D,Vec3 random_p3D )
        { 
            //random_p2D in [0,1]²
            //random_p3D in [0,1]³
            
            {func(incident_ray)}->std::convertible_to<std::pair<
                                                        Spectrum, //sampled Value
                                                        Prec       //Pdf
                                                        >>; 
            {func(&incident_ray,random_p2D)}->std::convertible_to<std::pair<Spectrum,Prec>>; 
            {func(&incident_ray,random_p3D)}->std::convertible_to<std::pair<Spectrum,Prec>>; 
            {IsDeltaDistribution(func)} -> std::convertible_to<bool>;
        };

        enum class MaterialType
        {
            BSDF,
            DIFFUSE,
            GLOSSY,
            Transmission,
            SUBSURFACE,
            EMISSION,
        };

        template<class T>
        auto CreateDistribution(T creator,const Ray& scattering_ray,const SurfaceInteraction& interaction)
        {
            return creator.CreateDistribution(scattering_ray,interaction);
        }

        template<class T, MaterialType type>
        concept DistributionCreator = requires(T creator,const Ray& scattering_ray,const SurfaceInteraction& interaction)
        {
            {CreateDistribution(creator, scattering_ray, interaction)}->DistributionFunction;
        };

        
    }
}