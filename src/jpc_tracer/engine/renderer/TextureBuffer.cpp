#include "TextureBuffer.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/renderer/JPCRenderer.h"
#include "jpc_tracer/engine/utilities/ImageIO.h"
#include <algorithm>



namespace jpctracer::renderer
{
    Texture::Texture(const Vec3* pixels, uint width, uint height) 
        : m_pixels(pixels), m_width(width), m_height(height)
    {
        
    }
    

    
    Texture TextureBuffer::Load(std::string filepath) 
    {
        uint width;
        uint height;
        const Vec3* pixels;
        if(! m_textures.contains(filepath))
            m_textures.emplace(filepath,std::make_unique<utility::Image>(std::move(utility::ReadImage(filepath))));
        
        const auto& image = m_textures[filepath]; 
        pixels = &image->rgbs[0];
        width = image->width;
        height = image->height;
        return Texture(pixels,width,height);

    }
    
    Texture TextureBuffer::DefaultTexture() 
    {
        return Texture(&m_default_color,1,1);
    }



}