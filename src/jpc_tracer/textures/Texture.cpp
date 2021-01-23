#include "Texture.h"



namespace jpc_tracer
{
    Texture::Texture(Ref<std::vector<double>> pixels, int width, int height) : _pixels(pixels), _width(width), _height(height)
    {
        
    }
    
    Vec3 Texture::GetPixel(int x, int y) 
    {
        int idx = (x*_width+y)*3;
        return Vec3((*_pixels)[idx],(*_pixels)[idx+1],(*_pixels)[idx+1]);
    }
    
    Vec3 Texture::GetPixel(Vec2 uv) 
    {
        int x = (int)(uv[0]*_width)%_width;
        int y = (int)(uv[1]*_height)%_height;

        if(x == _width)
            x = 0;
        else if(x < 0)
            x += _width;
            
        if(y == _height)
            y = 0;
        else if(y < 0)
            y += _height;

        int idx = (y*_width+x)*3;
        return Vec3((*_pixels)[idx],(*_pixels)[idx+1],(*_pixels)[idx+2]);
        //return Vec3(x/(Prec)_width, y/(Prec)_height, 0);
    }
}