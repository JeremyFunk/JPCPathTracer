#pragma once

#include "raytracing_core/include/Geometry.h"
#include "PrivateGeometry.h"
namespace jpc_rt_core
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