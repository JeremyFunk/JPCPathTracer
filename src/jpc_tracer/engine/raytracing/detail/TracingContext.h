#pragma once
#include "jpc_tracer/engine/raytracing/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/shade_programs/ShadePrograms.h"

namespace jpctracer::raytracing
{
    struct TracingContext
    {
        ShadePrograms Programs;
    };
}