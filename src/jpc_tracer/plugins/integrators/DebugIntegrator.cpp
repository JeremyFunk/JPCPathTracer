#include "DebugIntegrator.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <vector>



namespace jpctracer
{
    struct DebugPayload 
    {
        Vec3 scattering_dir;
    };
    void DebugIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
                Tracer& tracer, film::Film& film) const
    {
        Ray ray = camera->Generate(pixel, {0.5,0.5});
        DebugPayload payload;
        tracer(DebugBehavior(),ray,(Payload*)&payload);
        film.SavePixel("ScatteringRay", pixel, Vec3ToSpec(payload.scattering_dir));
        Prec length = payload.scattering_dir.norm();
        Spectrum val = std::abs(length-1)<0.001 ? FromValue(1) : FromValue(0); 
        film.SavePixel("ScatteringRayIs1", pixel,val);
        film.SavePixel("ScatteringRayZ", pixel, FromValue(payload.scattering_dir[2]*0.5+0.5));
    }
    
    void DebugBehavior::ClosestHitProgram(const HitPoint& hit_point, Payload* payload ,Tracer& trace) const
    {
        DebugPayload* d_payload = (DebugPayload*) payload;
        Vec2 sample = {0.5,0.5};

        auto results = hit_point.Shader<MATERIAL_BSDF>(View<Ray>{nullptr,0},{&sample,1});
        
        d_payload->scattering_dir = results.sampled_rays[0].Direction;
    }
    
    void DebugBehavior::Miss(const Spectrum& background_color, Payload* payload) const
    {
        DebugPayload* d_payload = (DebugPayload*) payload;
        d_payload->scattering_dir={-1,-1,-1};
    }
}