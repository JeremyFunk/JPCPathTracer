#include "BasicFilm.h"
#include "core/Spectrum.h"
#include "utilities/ImageIO.h"


namespace jpc_tracer
{
    void BasicFilm::SaveFilmTile(const FilmTile& tile) 
    {
        for(int y = tile.Bounds.YMin; y < tile.Bounds.YMax; y++)
        {
            for(int x = tile.Bounds.XMin; x < tile.Bounds.XMax; x++)
            {
                SpectrumPasses passes = tile.GetSpectrumAt(x, y);
                Spectrum spectrum = passes.GetCombined();
                Vec3 rgb =  spectrum.Clamp(0, 1).ToRGB();
                _pixels[y*_width*3+x*3] = rgb[0]*255;
                _pixels[y*_width*3+x*3+1] = rgb[1]*255;
                _pixels[y*_width*3+x*3+2] = rgb[2]*255;
                
            }
        }

    }

    void BasicFilm::WriteImage(std::string path) const
    {
        ::jpc_tracer::WriteImage(path, _pixels.data(), _width, _height);
    }

    BasicFilm::BasicFilm(int width,int height) 
        : _width(width),_height(height), _pixels(width*height*3) 
    {
        
    }
}
