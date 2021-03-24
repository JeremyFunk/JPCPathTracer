#pragma once

#include "jpc_tracer/core/core.h"

namespace jpctracer::shader::detail {
    
    struct SharedMemory
    {
        SurfaceInteraction Interaction;
        //in normal space
        Vec3 ScatteringDir;

        //in object space
        Ray ScatteringRay;

        //Normal Space World Matrix
        Vec3 Normal;
        Vec3 Tangent1;
        Vec3 Tangent2;
    };

    Vec3 WorldToNormal(const Vec3& x, const SharedMemory& mem);

    Ray WorldToNormal(const Ray& x, const SharedMemory& mem);
    
    Vec3 NormalToWorld(const Vec3& x,const SharedMemory& mem);

    Ray NormalToWorld(const Ray& x, const SharedMemory& mem);

    SharedMemory CreateSharedMemory(const Ray& scattering_ray,
                                const SurfaceInteraction& interaction);

}