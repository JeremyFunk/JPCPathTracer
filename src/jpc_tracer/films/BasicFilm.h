#pragma once
#include "core/IFilm.h"
#include <vector>

namespace jpc_tracer
{
    class BasicFilm : public IFilm
    {
    public:
        BasicFilm(int width,int height);
        virtual void SaveFilmTile(const FilmTile& tile);

        virtual void WriteImage(std::string path) const;

        virtual int GetWidth() {return _width;};
        virtual int GetHeight() {return _height;};

    private:
        std::vector<unsigned char> _pixels = {0};
        int _width;
        int _height;

    };
}