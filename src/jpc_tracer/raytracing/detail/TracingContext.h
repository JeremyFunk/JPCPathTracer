#pragma once
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadePrograms.h"

namespace jpctracer::raytracing
{
    struct TracingContext
    {
        ShadePrograms Programs;
    };
}