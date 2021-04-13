#pragma once

#include "jpc_tracer/core/core.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "FilmChannel.h"
namespace jpctracer::film
{
    class BasicFilm
    {
    public:
        BasicFilm(uint width, uint height);
        void SavePixel(std::string channel,uint x, uint y, Spectrum val);
        void WriteChannels(std::string directory);
    private:
        std::mutex m_channel_mutex;
        std::unordered_map<std::string, std::unique_ptr<FilmChannel>> m_film_channels;
        uint m_width;
        uint m_height;
        
    };


    static_assert(cts::Film<BasicFilm>);
}