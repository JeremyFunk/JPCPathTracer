#pragma once
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "maths/maths.h"
#include <concepts>
#include <iterator>

namespace jpctracer
{

struct Payload;
struct AnyHitResult
{
    bool IsHit;
    bool ShouldTerminate = false;
};

} // namespace jpctracer