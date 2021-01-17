#pragma once
#include "core/Linalg.h"
#include <memory.h>
#include <memory>

namespace textures
{
    class Texture
    {
    public:
        Texture(std::shared_ptr<std::vector<double>> pixels, int width, int height);
        core::Vec3 GetPixel(int x, int y);
        core::Vec3 GetPixel(core::Vec2 uv);
    private:
        std::shared_ptr<std::vector<double>> _pixels;
        int _width;
        int _height;
    };
}