#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include <stdint.h>

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

        //return __lzcnt(combined); //MSVC 
        return __builtin_clz(combined);
    }
}