#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <stdint.h>

#ifdef __clang__
    #define clz(x) __builtin_clz(x)

#elif defined _MSC_VER
    #define clz(x) __lzcnt(x)

#else 
    static uint32_t ALWAYS_INLINE popcnt(uint32_t x)
    {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    static uint32_t ALWAYS_INLINE clz(uint32_t x)
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - popcnt(x);
    }
#endif

namespace jpctracer::raytracing
{
    inline const uint32_t LeftShift3(uint32_t x)
    {
        x = (x * 0x00010001u) & 0xFF0000FFu;
        x = (x * 0x00000101u) & 0x0F00F00Fu;
        x = (x * 0x00000011u) & 0xC30C30C3u;
        x = (x * 0x00000005u) & 0x49249249u;
        return x;
    }

    inline const uint32_t EncodeMorton(const Vec3& centroid)
    {
        const uint32_t x = LeftShift3(std::min(std::max(centroid[0] * 1024.0f, 0.0f), 1023.0f));
        const uint32_t y = LeftShift3(std::min(std::max(centroid[1] * 1024.0f, 0.0f), 1023.0f));
        const uint32_t z = LeftShift3(std::min(std::max(centroid[2] * 1024.0f, 0.0f), 1023.0f));

        return x * 4 + y * 2 + z;
    }
    
    inline int CountLeadingZerosCombined(const uint32_t& a, const uint32_t& b)
    {
        const uint32_t combined = a ^ b;

        if (combined == 0)  return 32U;

        return clz(combined);
    }

    int number_leading_zeros(uint32_t* morton, const int& idx, const int offset, const size_t& number_nodes);
    int calc_direction(uint32_t* morton, const int& idx, const size_t& number_nodes);
    uint calc_last_idx(uint32_t* morton, const int& idx, const int& dir, const size_t& number_nodes);
    uint calc_split_idx(uint32_t* morton, const int& min_idx, const int& max_idx, const int& dir, const size_t& number_nodes);
    Bounds3D calc_bounds(Bounds3D* bound_begin, const Bounds3D* bound_end);
    Bounds3D calc_bounds(Bounds3D* bounds, const uint& min_idx, const uint& max_idx);
    }