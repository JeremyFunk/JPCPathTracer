#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{
struct MirrorBsdfClosure final : public IBsdfClosure
{
    // incident_dir in normal space
    MirrorBsdfClosure(Ray scattering_ray) : m_mirror_ray(scattering_ray)
    {

        m_mirror_ray.Direction.flip(0);
        m_mirror_ray.Direction.flip(1);
    }
    inline Distributed<Spectrum> Eval(Ray incident_ray) const
    {
        if (incident_ray.Direction.IsNear(m_mirror_ray.Direction))
            return {FromValue(1), 0};
        return {Black(), 0};
    }
    inline Norm3 Sample2D(Vec2 rand_p) const
    {
        return m_mirror_ray.Direction;
    }

  private:
    Ray m_mirror_ray;
};

inline BsdfNode MirrorBsdf(Ray scattered)
{
    return CreateBsdf(MATERIAL_GLOSSY, MirrorBsdfClosure(scattered));
}
} // namespace jpctracer