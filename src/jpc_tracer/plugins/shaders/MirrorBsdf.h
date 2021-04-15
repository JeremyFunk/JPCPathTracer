#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer {
    struct MirrorBsdfClosure : public IBsdfClosure
    {
        //incident_dir in normal space
        MirrorBsdfClosure(Ray scattering_ray)
        {
            Vec3 scattering_dir = scattering_ray.Direction;
            Prec time = scattering_ray.Time;   
            m_mirror_ray.Direction[0]= scattering_dir[0]*-1;
            m_mirror_ray.Direction[1]=scattering_dir[1]*-1;
            m_mirror_ray.Direction[2] = scattering_dir[2];
            m_mirror_ray.Time = time;
            m_mirror_ray.Origin = {0,0,0};
        }
        inline ShaderResult Eval( Ray incident_ray) const
        {
            //if(incident_ray.Direction == m_mirror_ray.Direction)
            //    return {FromValue(1),0};
            return {Black(),0};
        }
        inline ShaderResult Sample2D( Ray* out_incident_ray,Vec2 rand_p) const
        {
            *out_incident_ray = m_mirror_ray;
            return {FromValue(1),0};
        }
    private:
        Ray m_mirror_ray;
    };

    constexpr BsdfNode MirrorBsdf(ShaderContext* ctx)
    {
        return CreateBsdf<MATERIAL_GLOSSY, MirrorBsdfClosure>(ctx, ctx->normal_space.ScatteringRay);
    }
}