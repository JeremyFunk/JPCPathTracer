#include "core/Bounds2D.h"
#include "core/FilmTile.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "films/BasicFilm.h"
#include "gtest/gtest.h"
#include <memory>
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
namespace films {

    TEST(film,test)
    {
        int width = 5;
        int height = 2;

        core::Bounds2D<int> bounds(width,0,height,0);
        auto filter = std::make_shared<filters::GaussianFilter>(1);
        core::FilmTile tile(filter,bounds);

        std::vector<core::SpectrumPasses> passes;
        for(int i = 0; i < width*height;i++)
        {
            float val = (float) i / (width*height);
            passes.push_back(core::SpectrumPasses(core::Spectrum::FromRGB({val,val,val})));    
        }
        for (int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int idx = y * width + x;
                tile.AddSample(passes[idx], {x,y});
            }
        }

        films::BasicFilm film(width,height);
        film.SaveFilmTile(tile);
        film.WriteImage("film_test.png");

    }
}
       