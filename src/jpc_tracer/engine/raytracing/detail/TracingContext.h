#pragma once
#include "Scene.h"
#include "ShadePrograms.h"

namespace jpctracer::raytracing
{
    struct TracingContext
    {
        ShadePrograms programs;
        const Scene* scene;
    };
}