#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/maths.h"

namespace jpctracer {
    namespace archetypes {
        //Archetypes are reference classes, for generic Programming
        //They must not be used in real code

        //Camera
        //**********************************************************************

        struct CameraFunction
        {
            Ray operator()(Vec2 pixel, Vec2 random_point){ return Ray();}
        };

        struct CameraBuilder
        {
            CameraFunction Build() {return {};}
        };

        //Sampler
        //**********************************************************************

        
        struct SamplerFunction
        {
            template<std::output_iterator<Vec2> ResultIT>
            void operator()(UInt2 counts, ResultIT it) {}

            template<std::output_iterator<Vec3> ResultIT>
            void operator()(UInt3 counts, ResultIT it) {}
        };

        struct SamplerSettings
        {
            SamplerFunction Build(){return {};}
        };

        //Shader
        //**********************************************************************

        struct DistributionFunction
        {
            DistributionFunction() = default;
            DistributionFunction(const DistributionFunction& o) = delete;
            DistributionFunction(DistributionFunction&& o) = delete;

            std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const
            {return {};}

            std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
            {return {};}

            std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                                Vec3 random_point) const
            {return {};}
        };

        
    }
    namespace archetypes
    {

        

        //Integrator
        //**********************************************************************

        struct Payload;

        //Forward declaration
        struct TraceRay{
            template<MaterialType type>
            void operator()(Ray ray,Payload* payload);
        };

        struct Film
        {
            void SavePixel(std::string channel_name, uint x, uint y, Vec3 rgb)
            {}
        };

        
        
        //End forward declaration
        /*
        struct RayBehavior
        {
            //returnes if it was an hit
            template<class _HitPoint>
            bool AnyHitProgram(const _HitPoint& hit_point,Payload* payload);

            template<class _HitPoint>
            void ClosestHitProgram(const _HitPoint& hit_point, Payload* payload ,TraceRay& trace);

            template<class BackbroundDistribution>
            void Miss(const BackbroundDistribution& background, Payload* payload);
        };

        struct Integrator
        {
            template<class CameraSettings, class Sampler, class PixelSaver>
            void operator()(Int2 pixel, const CameraSettings& camera, 
                    Sampler& sampler,TraceRay& trace, PixelSaver& saver)
            {}

            template<MaterialType type>
            RayBehavior GetRayBehavior() const {return{};}

        };
        
        struct IntegratorBuilder
        {
            Integrator Build() {return{};}
        };*/
    }
}