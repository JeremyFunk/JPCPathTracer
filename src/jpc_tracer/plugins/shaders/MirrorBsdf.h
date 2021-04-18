#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer {
    struct MirrorBsdfClosure : public IBsdfClosure
    {
        //incident_dir in normal space
        MirrorBsdfClosure(Ray scattering_ray)
            : m_mirror_ray(scattering_ray)
        {

            m_mirror_ray.Direction.flip(0);
            m_mirror_ray.Direction.flip(1);
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

    constexpr BsdfNode MirrorBsdf(ShaderContext& ctx)
    {
        return CreateBsdf<MATERIAL_GLOSSY, MirrorBsdfClosure>(ctx, ctx.normal_space.ScatteringRay);
    }
}