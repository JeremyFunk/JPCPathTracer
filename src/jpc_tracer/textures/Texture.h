#pragma once
#include "core/Linalg.h"
#include <memory.h>
#include <memory>

namespace jpc_tracer
{
    class Texture
    {
    public:
        Texture(Ref<std::vector<double>> pixels, int width, int height);
        Vec3 GetPixel(int x, int y);
        Vec3 GetPixel(Vec2 uv);
    private:
        Ref<std::vector<double>> _pixels;
        int _width;
        int _height;
    };
}