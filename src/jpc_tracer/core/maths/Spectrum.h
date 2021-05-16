#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Vec.h"
namespace jpctracer
{
using Spectrum = math::Vec<Prec, 4>;

Prec& Transparency(Spectrum& spec);
bool IsBlack(Spectrum spec);
Spectrum Black();
Spectrum FromValue(const Prec& v);
Spectrum Vec3ToSpec(const Vec3& b);

namespace math
{

Spectrum operator*(const Spectrum& spec1, const Spectrum& spec2);
void operator*=(Spectrum& spec1, const Spectrum& spec2);

} // namespace math

namespace srgb
{

Vec3 ToRGB(const Spectrum& spec) noexcept;
Spectrum FromRGB(const Vec3& rgb) noexcept;

} // namespace srgb

namespace xyz
{

Vec3 ToRGB(const Spectrum& spec) noexcept;
Spectrum FromRGB(const Vec3& rgb) noexcept;

} // namespace xyz

using namespace srgb;

} // namespace jpctracer