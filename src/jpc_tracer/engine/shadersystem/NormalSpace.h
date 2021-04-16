#pragma once

#include "jpc_tracer/core/core.h"

namespace jpctracer::shadersys {
    
    struct NormalSpace
    {
        SurfaceInteraction Interaction;
        //in normal space
        Ray ScatteringRay;

        //Normal Space World Matrix
        Vec3 Normal;
        Vec3 Tangent1;
        Vec3 Tangent2;
    };

    Vec3 WorldToNormal(const Vec3& x, const NormalSpace& mem);

    Ray WorldToNormal(const Ray& x, const NormalSpace& mem);
    
    Vec3 NormalToWorld(const Vec3& x,const NormalSpace& mem);

    Ray NormalToWorld(const Ray& x, const NormalSpace& mem);

    NormalSpace CreateNormalSpace(const Ray& scattering_ray,
                                const SurfaceInteraction& interaction);

}