#pragma once
#include "Scene.h"

namespace jpctracer::raytracing
{
    struct TracingContext
    {
        const Scene* scene;
    };
}