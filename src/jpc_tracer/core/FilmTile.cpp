#include "FilmTile.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <iostream>

namespace jpc_tracer {
    FilmTile::FilmTile(std::shared_ptr<IFilter> filter, Bounds2D<int> bounds) 
        : Bounds(bounds), _filter(filter)
    {
        _width = Bounds.XMax-Bounds.XMin;
        int size = bounds.Size();
        _weights.resize(size);
        _pixel_unnormalized.resize(size);
    }
    

    
    void FilmTile::AddSample(const SpectrumPasses& spectrum,Vec2 image_position,Prec weight) 
    {
        int pixel_x = floor(image_position[0]);
        int pixel_y = floor(image_position[1]);
        int x_min = std::max(pixel_x+_filter->BoundingBox.XMin,Bounds.XMin);
        int x_max = std::min(pixel_x+_filter->BoundingBox.XMax,Bounds.XMax);  
        int y_min = std::max(pixel_y+_filter->BoundingBox.YMin,Bounds.YMin);
        int y_max = std::min(pixel_y+_filter->BoundingBox.YMax,Bounds.YMax);

        

        for(int y = y_min; y < y_max; y++)
        {
            for(int x = x_min; x < x_max; x++)
            {
                Prec diff_x = image_position[0]-(Prec)x;
                Prec diff_y = image_position[1]-(Prec)y;
                Prec filter_val = _filter->Evaluate({diff_x,diff_y});
                add_to_weight(x, y, filter_val);
                add_to_pixel_unnormalized(x, y, spectrum*filter_val*weight);
            }
        }
    }
    
    SpectrumPasses FilmTile::GetSpectrumAt(int x,int y) const 
    {
        int tile_space_x = x-Bounds.XMin;
        int tile_space_y = y-Bounds.YMin;
        return _pixel_unnormalized[tile_space_x+tile_space_y*_width]/_weights[tile_space_x+tile_space_y*_width];
    }
    
    void FilmTile::add_to_weight(int x,int y, Prec weight) 
    {
        _weights[(y-Bounds.YMin)*_width+(x-Bounds.XMin)] += weight;
    }
    
    void FilmTile::add_to_pixel_unnormalized(int x, int y, SpectrumPasses pixel) 
    {
        int idx = (y - Bounds.YMin) * _width + (x - Bounds.XMin);
        _pixel_unnormalized[idx] += pixel;
    }

    


}