#pragma once
#include "raytracing_core/include/Scene.h"
#include "raytracing_core/src/shade_programs/ShadePrograms.h"

namespace jpc_rt_core
{
    struct TracingContext
    {
        ShadePrograms Programs;
    };
}