#pragma once
#include "jpc_tracer/raytracing/Geometry.h"
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/detail/geometry/PrivateRootGeometry.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadingData.h"


namespace jpctracer::raytracing {
    struct PrivateScene
    {
        PrivateRootGeometry GeometryRoot; 
        ShadingData Shading;

    };
}