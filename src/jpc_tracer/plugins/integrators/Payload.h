#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"

namespace jpctracer {
    struct Payload{
        Vec2* samples;
        Spectrum throughput = FromValue(1);
        Spectrum result = FromValue(0);
        bool IsShadow = false;
    };
}