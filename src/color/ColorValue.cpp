#include "ColorValue.h"



namespace color
{
    ColorValueVec3::ColorValueVec3(core::Vec3 color) : _color(color)
    {}
    
    core::Vec3 ColorValueVec3::GetColor(core::Vec2 uv) const
    {
        return _color;
    }
    
    ColorValueTexture::ColorValueTexture(std::shared_ptr<textures::Texture> texture) : _texture(texture)
    {}
    
    core::Vec3 ColorValueTexture::GetColor(core::Vec2 uv) const
    {
        return _texture->GetPixel(uv);
    }
}