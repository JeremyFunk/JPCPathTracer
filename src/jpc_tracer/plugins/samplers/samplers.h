#pragma once

#include "GridSampler.h"

namespace jpctracer::sampler {
    template<class T>
    auto Build(T builder) {return builder.Build();}
}
