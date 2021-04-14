#pragma once

#include "jpc_tracer/core/core.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "FilmChannel.h"
namespace jpctracer::film
{
    class Film
    {
    public:
        Film(uint width, uint height);
        void SavePixel(std::string channel,UInt2 pixel, Spectrum val);
        void WriteChannels(std::string directory);
    private:
        std::mutex m_channel_mutex;
        std::unordered_map<std::string, std::unique_ptr<FilmChannel>> m_film_channels;
        uint m_width;
        uint m_height;
        
    };


}