#pragma once
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/geometry/PrivateRootGeometry.h"
#include "jpc_tracer/engine/raytracing/detail/shade_programs/ShadingData.h"


namespace jpctracer::raytracing {
    struct PrivateScene
    {
        PrivateRootGeometry GeometryRoot; 
        ShadingData Shading;

    };
}