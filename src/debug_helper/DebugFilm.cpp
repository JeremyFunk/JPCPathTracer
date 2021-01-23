#include "DebugFilm.h"
#include "utilities/ImageIO.h"

namespace jpc_tracer {
    DebugFilm::DebugFilm(int width, int height) 
        : _width(width), _height(height)
    {
        _pixels = std::vector<float>(_width*height*3);
        for(float& pixel : _pixels)
            pixel = 0;
        
    }

    void DebugFilm::AddValue(int x, int y, float value) 
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _pixels[y*_width*3+x*3] += value;
        _pixels[y*_width*3+x*3+1] += value;
        _pixels[y*_width*3+x*3+2] += value;
        float p = _pixels[y * _width * 3 + x * 3];
        if(p >_max_val) _max_val = p;
    }
    
    void DebugFilm::AddColor(int x, int y, Spectrum spec) 
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        Vec3 rgb = spec.ToRGB();
        _pixels[y*_width*3+x*3] += rgb[0];
        _pixels[y*_width*3+x*3+1] += rgb[1];
        _pixels[y*_width*3+x*3+2] += rgb[2];

        float r = _pixels[y * _width * 3 + x * 3];
        float g = _pixels[y * _width * 3 + x * 3 + 1];
        float b = _pixels[y * _width * 3 + x * 3 + 2];
        _max_val = std::max<float>({ r,g,b,_max_val});
    }

    void DebugFilm::SetValue(int x, int y, float value)
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _pixels[y * _width * 3 + x * 3] = value;
        _pixels[y * _width * 3 + x * 3 + 1] = value;
        _pixels[y * _width * 3 + x * 3 + 2] = value;

        _max_val = std::max<float>(value,_max_val);
    }
    
    void DebugFilm::SetColor(int x, int y, Spectrum spec)
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        Vec3 rgb = spec.ToRGB();
        _pixels[y * _width * 3 + x * 3] = rgb[0];
        _pixels[y * _width * 3 + x * 3 + 1] = rgb[1];
        _pixels[y * _width * 3 + x * 3 + 2] = rgb[2];

        float r = _pixels[y * _width * 3 + x * 3];
        float g = _pixels[y * _width * 3 + x * 3 + 1];
        float b = _pixels[y * _width * 3 + x * 3 + 2];
        _max_val = std::max<float>({ r,g,b,_max_val });

    }

    void DebugFilm::Save(std::string filepath)
    {
        auto conv_pixels = std::make_unique<std::vector<unsigned char>>(_width*_height*3);
        for(int i = 0 ;i < conv_pixels->size();i++)
        {
            conv_pixels->operator[](i) =_pixels[i] / _max_val * 255.0f;
        }
        WriteImage(filepath, conv_pixels->data(), _width, _height);
    }
    
    void DebugFilm::SaveLog(std::string filepath)
    {
        auto conv_pixels = std::make_unique<std::vector<unsigned char>>(_width * _height * 3);
        for (int i = 0; i < conv_pixels->size(); i++)
        {
            conv_pixels->operator[](i) = std::log(1 + _pixels[i]) * 255.0f;
        }
        WriteImage(filepath, conv_pixels->data(), _width, _height);
    }
}