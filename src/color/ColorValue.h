#pragma once
#include "core/Linalg.h"
#include "textures/Texture.h"

namespace color
{
    class ColorValue
    {
    public:
        virtual core::Vec3 GetColor(core::Vec2 uv) const = 0;
    };

    class ColorValueVec3 : public ColorValue{
    public:
        ColorValueVec3(core::Vec3 color);
        virtual core::Vec3 GetColor(core::Vec2 uv) const override;
    private:
        core::Vec3 _color;
    };

    class ColorValueTexture : public ColorValue{
    public:
        ColorValueTexture(std::shared_ptr<textures::Texture> texture);
        virtual core::Vec3 GetColor(core::Vec2 uv) const override;
    private:
        std::shared_ptr<textures::Texture> _texture;
    };
}