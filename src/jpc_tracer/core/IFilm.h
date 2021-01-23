#pragma once
#include"Base.h"
#include"Linalg.h"
#include "Spectrum.h"
#include <algorithm>
#include <string>
#include <vector>
#include "FilmTile.h"
#include "core/SpectrumPasses.h"
#include "memory"
namespace jpc_tracer {
    class IFilm
    {
    public:
        virtual void SaveFilmTile(const FilmTile& tile) = 0;

        virtual void WriteImage(std::string path) const = 0;

        virtual int GetWidth() = 0;
        virtual int GetHeight() = 0;
        

        
    };
}