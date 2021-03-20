#pragma once

#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "PrivateGeometry.h"
namespace jpctracer::raytracing
{
    struct PrivateRootGeometry
    {
        InstanceBVHType instance_bvh_type;
        StaticBVHType static_bvh_type;
        DynamicBVHType dynamic_bvh_type;

        std::vector<PrivateGeometry> StaticGeometries; 

        ~PrivateRootGeometry();
    };
}