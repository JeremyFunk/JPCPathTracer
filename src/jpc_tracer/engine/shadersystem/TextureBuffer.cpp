#include "TextureBuffer.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/utilities/ImageIO.h"
#include <algorithm>

namespace jpctracer::shadersys
{
Texture::Texture(const Vec3* pixels, uint width, uint height) : m_pixels(pixels), m_width(width), m_height(height)
{
}

Texture TextureBuffer::Load(std::string filepath)
{
    uint width;
    uint height;
    const Vec3* pixels;
    if (!m_textures.contains(filepath))
        m_textures.emplace(filepath, std::make_unique<utility::Image>(std::move(utility::ReadImage(filepath))));

    const auto& image = m_textures[filepath];
    pixels = &image->rgbs[0];
    width = image->width;
    height = image->height;
    return Texture(pixels, width, height);
}

Texture TextureBuffer::DefaultTexture()
{
    return Texture(&m_default_color, 1, 1);
}

RGB Texture::operator()(Vec2 uv) const
{
    // error
    auto [u, v] = uv.Data;
    // JPC_LOG_INFO("A UV: V: {}",v);
    uint x_lower = u * m_width;
    uint x_upper = ceil(u * m_width);
    uint y_lower = v * m_height;
    uint y_upper = ceil(v * m_height);

    x_lower = std::clamp<uint>(x_lower, 0, m_width - 1);
    x_upper = std::clamp<uint>(x_upper, 0, m_width - 1);
    y_lower = std::clamp<uint>(y_lower, 0, m_height - 1);
    y_upper = std::clamp<uint>(y_upper, 0, m_height - 1);

    Vec3 lower_lower = m_pixels[y_lower * m_width + x_lower];
    Vec3 lower_upper = m_pixels[y_upper * m_width + x_lower];
    Vec3 upper_lower = m_pixels[y_lower * m_width + x_upper];
    Vec3 upper_upper = m_pixels[y_upper * m_width + x_upper];
    // JPC_LOG_INFO("Coords: Y: {}",y_lower);

    auto result = (lower_lower + lower_upper + upper_lower + upper_upper) / 4.f;
    // auto result = lower_lower;
    // if(abs(result[0]-result[1])>0.2 || abs(result[0]-result[2])>0.2)
    //    JPC_LOG_INFO("Different Color: {},{},{}",result[0],result[1],result[2]);
    return {result};
}
} // namespace jpctracer::shadersys