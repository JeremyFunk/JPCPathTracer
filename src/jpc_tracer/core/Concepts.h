#pragma once
#include <concepts>
#include <iterator>
#include "jpc_tracer/core/maths/Constants.h"
#include "maths/maths.h"
#include "archetypes.h"

namespace jpctracer {

    //plugins
    namespace cts {

        //Samplers
        //**********************************************************************

        template<class T, class ResultIterator>
        concept SamplerFunction2D = std::invocable<T,UInt2,ResultIterator> 
            && std::output_iterator<ResultIterator,Vec2>;

        template<class T, class ResultIterator>
        concept SamplerFunction3D = std::invocable<T,UInt3,ResultIterator>
            && std::output_iterator<ResultIterator,Vec3>;
        
        template<class T,class ResultIt2D,class ResultIt3D>
        concept SamplerFunction = SamplerFunction2D<T,ResultIt2D>
                                && SamplerFunction3D<T,ResultIt3D>;
        
        //Cameras
        //**********************************************************************
        template<class T>
        concept CameraFunction = requires (const T camera,Vec2 pixel,
                                            Vec2 random_point)
        {
            {camera(pixel,random_point)}->std::convertible_to<Ray>;
        };

        //Shaders
        //**********************************************************************
        
        template<class T>
        concept DistributionFunction = requires(const T& func, Ray incident_ray,
                                                Vec2 random_p2D,Vec3 random_p3D)
        { 
            //random_p2D in [0,1]²
            //random_p3D in [0,1]³
            
            {func(incident_ray)}->std::convertible_to<std::pair<
                                                        Spectrum, //sampled Value
                                                        Prec       //Pdf
                                                        >>; 
            {func(&incident_ray,random_p2D)}
                                ->std::convertible_to<std::pair<Spectrum,Prec>>; 
            {func(&incident_ray,random_p3D)}
                                ->std::convertible_to<std::pair<Spectrum,Prec>>; 
        };

        inline bool IsDeltaDistribution(Prec pdf) {return std::abs(pdf)<0.0001;}



        template<class T>
        concept DistributionCreator = requires(T creator,const Ray& scattering_ray,
                                        const SurfaceInteraction& interaction)
        {
            {CreateDistribution<MaterialType::BSDF>(creator, scattering_ray, 
                            interaction)}
                ->DistributionFunction;
        };

        
        //Film
        //**********************************************************************
        template<class T>
        concept PixelSaver = requires(T saver, std::string channel_name,
                                            uint x, uint y, Vec3 rgb)
        {
            saver(channel_name,x,y,rgb);
        };

        //Integrator
        //**********************************************************************
        struct Payload;

        template<class T>
        concept TraceRay = requires(T tracer,Ray ray, Payload* payload)
        {
            tracer<MaterialType::BSDF>(ray,payload);
            tracer<MaterialType::DIFFUSE>(ray,payload);
            tracer<MaterialType::EMISSION>(ray,payload);
            tracer<MaterialType::GLOSSY>(ray,payload);
            tracer<MaterialType::SUBSURFACE>(ray,payload);
            tracer<MaterialType::TRANSMISSION>(ray,payload);
            tracer<MaterialType::TRANSLUCENT>(ray,payload);
        };
        

        template<class T>
        concept RayBehavior = requires(T behavior, 
                    const shader::LightsDistribution& lights, 
                    const archetypes::DistributionFunction& material,
                    const archetypes::DistributionFunction& material_emission,
                    const archetypes::DistributionFunction& background,
                    Payload* payload, archetypes::TraceRay& tracer)
        {
            {behavior.AnyHitProgram(material,payload)} -> std::convertible_to<bool>;
            {behavior.ClosestHitProgram(lights,material,material_emission,
                        payload,tracer)};
            {behavior.Miss(background,payload)};
        };

        template<MaterialType type, class T>
        auto GetRayBehavior(const T& integrator)
        {
            return integrator.template GetRayBehavior<type>();
        }

        template<class T, class Camera,class Sampler, class Film, class Tracer>            
        concept Integrator = requires(T integrator,const Camera& camera,
                            Sampler& sampler, Film film, Tracer& tracer)
        {
            integrator(camera,sampler,film);
            {GetRayBehavior<MaterialType::BSDF>(integrator)}
            -> RayBehavior;

        }   
        && CameraFunction<Camera>
        && SamplerFunction<Sampler,Vec2*,Vec3*>
        && PixelSaver<Film>
        && TraceRay<Tracer>;

        //Samplers
        //**********************************************************************
        
        template<class T,class ResultIt2D,class ResultIt3D>
        concept SamplerBuilder = requires(T builder)
        {
            {Build(builder)}->SamplerFunction<ResultIt2D,ResultIt3D>;
        };

        //Cameras
        //**********************************************************************
        template<class T>
        concept CameraBuilder = requires(T builder)
        {
            {Build(builder)} -> CameraFunction;
        };

        //Shaders
        //**********************************************************************

        template<class T>
        concept ShaderBuilder = requires(T builder, shader::ShaderCache& cache)
        {
            {Build(builder,cache)} -> DistributionCreator;
        };

        //Integrator
        //**********************************************************************
        template<class T>
        concept IntegratorBuilder = requires(T builder)
        {
            {Build(builder)}->Integrator<archetypes::CameraFunction, 
                        archetypes::SamplerFunction, archetypes::PixelSaver, 
                        archetypes::TraceRay>;
        };

    }
}