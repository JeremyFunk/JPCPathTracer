#pragma once

#include "jpc_tracer/engine/PluginsApi.h"
namespace jpctracer
{
constexpr Prec Mix(Prec first, Prec second, Prec mix_factor)
{
    return first * mix_factor + second * (1 - mix_factor);
}

constexpr Vec3 Mix(Vec3 first, Vec3 second, Prec mix_factor)
{
    return first * mix_factor + second * (1 - mix_factor);
}

constexpr Spectrum Mix(Spectrum first, Spectrum second, Prec mix_factor)
{
    return first * mix_factor + second * (1 - mix_factor);
}

} // namespace jpctracer