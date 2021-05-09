#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/utilities/ImageIO.h"
#include <unordered_map>
namespace jpctracer::shadersys
{
struct RGB
{
    Vec3 data;
    operator Spectrum() const
    {
        return FromRGB(data);
    }
    operator Norm3() const
    {
        return data;
    }
    operator Vec3() const
    {
        return data;
    }
    operator Prec() const
    {
        return (data[0] + data[1] + data[2]) / 3.f;
    }
};

class Texture
{
  public:
    Texture(const Vec3* pixels, uint width, uint height);
    // Texture(Texture&&) = delete;
    RGB operator()(Vec2 uv) const;
    ~Texture()
    {
    }

  private:
    const Vec3* m_pixels;
    uint m_width;
    uint m_height;
};
class TextureBuffer
{
  public:
    Texture Load(std::string filepath);
    Texture DefaultTexture();

  private:
    std::unordered_map<std::string, std::unique_ptr<utility::Image>> m_textures;

    Vec3 m_default_color = {1, 0, 1};
};
} // namespace jpctracer::shadersys