#pragma once
#include "core/Linalg.h"
#include "textures/Texture.h"

namespace jpc_tracer
{
    class ColorValue
    {
    public:
        virtual Vec3 GetColor(Vec2 uv) const = 0;
    };

    class ColorValueVec3 : public ColorValue{
    public:
        ColorValueVec3(Vec3 color);
        virtual Vec3 GetColor(Vec2 uv) const override;
    private:
        Vec3 _color;
    };

    class ColorValueTexture : public ColorValue{
    public:
        ColorValueTexture(std::shared_ptr<Texture> texture);
        virtual Vec3 GetColor(Vec2 uv) const override;
    private:
        std::shared_ptr<Texture> _texture;
    };
}