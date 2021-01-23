#include "ColorValue.h"



namespace jpc_tracer
{
    ColorValueVec3::ColorValueVec3(Vec3 color) : _color(color)
    {}
    
    Vec3 ColorValueVec3::GetColor(Vec2 uv) const
    {
        return _color;
    }
    
    ColorValueTexture::ColorValueTexture(Ref<Texture> texture) : _texture(texture)
    {}
    
    Vec3 ColorValueTexture::GetColor(Vec2 uv) const
    {
        return _texture->GetPixel(uv);
    }
}