#include "FilmChannel.h"
#include "../utilities/ImageIO.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <algorithm>
#include <filesystem>


namespace jpctracer
{


    void FilmChannel::WriteImage(std::string directory) const
    {
        char sep = std::filesystem::path::preferred_separator;
        std::string path = directory+sep+m_name+".png";
        utility::WriteImage(path, m_pixels.data(), m_width, m_height);
    }

    FilmChannel::FilmChannel(std::string name,int width,int height) 
        : m_width(width),m_height(height), m_pixels(width*height*3),m_name(name)
    {
        
    }
    
    void FilmChannel::SavePixel(UInt2 pixel, Spectrum spec) 
    {
        uint x = pixel[0];
        uint y = pixel[1];
        
        Vec3 rgb = ToRGB(spec);
        rgb[0] = std::clamp<Prec>(rgb[0],0,1);
        rgb[0] = std::clamp<Prec>(rgb[0],0,1);
        rgb[0] = std::clamp<Prec>(rgb[0],0,1);
        m_pixels[3*(y*m_width+x)] = rgb[0]*255; 
        m_pixels[3*(y*m_width+x)] = rgb[1]*255;
        m_pixels[3*(y*m_width+x)] = rgb[2]*255;
    }

}
 
