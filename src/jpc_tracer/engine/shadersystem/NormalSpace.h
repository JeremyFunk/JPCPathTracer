#pragma once

#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"

namespace jpctracer::shadersys {
    
    struct NormalSpace
    {
        SurfaceInteraction Interaction;
        //in normal space
        Ray ScatteringRay;

        //Normal Space World Matrix
        Norm3 Normal;
        Norm3 Tangent1;
        Norm3 Tangent2;
    };

    Vec3 WorldToNormal(const Vec3& x, const NormalSpace& mem);

    Norm3 WorldToNormal(const Norm3& x, const NormalSpace& mem);

    Ray WorldToNormal(const Ray& x, const NormalSpace& mem);
    
    Vec3 NormalToWorld(const Vec3& x,const NormalSpace& mem);
    Norm3 NormalToWorld(const Norm3& x,const NormalSpace& mem);

    Norm3 NormalToWorld(const Norm3& x,const NormalSpace& mem);

    Ray NormalToWorld(const Ray& x, const NormalSpace& mem);

    NormalSpace CreateNormalSpace(const Ray& scattering_ray,
                                const SurfaceInteraction& interaction);

}