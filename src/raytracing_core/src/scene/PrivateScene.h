#pragma once
#include "raytracing_core/include/Geometry.h"
#include "raytracing_core/include/Scene.h"
#include "raytracing_core/src/geometry/PrivateRootGeometry.h"
#include "raytracing_core/src/shade_programs/ShadingData.h"


namespace jpc_rt_core {
    struct PrivateScene
    {
        PrivateRootGeometry GeometryRoot; 
        ShadingData Shading;

    };
}