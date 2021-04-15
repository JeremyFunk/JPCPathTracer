#include "Film.h"
#include "jpc_tracer/engine/films/FilmChannel.h"
#include <mutex>



namespace jpctracer::film
{
    Film::Film(uint width, uint height) 
        : m_width(width), m_height(height)
    {
        
    }

    void Film::SavePixel(std::string channel,UInt2 pixel, Spectrum val) 
    {
        //pretty hacky
        if(!m_film_channels.contains(channel))
        {
            m_channel_mutex.lock();
            if(!m_film_channels.contains(channel))
            {
                auto new_channel = std::make_unique<FilmChannel>(channel,m_width,m_height);
                m_film_channels.insert({channel,std::move(new_channel)});
            }
            m_channel_mutex.unlock();
        }
        m_film_channels.at(channel)->SavePixel(pixel, val);

    }
    
    void Film::WriteChannels(std::string directory) 
    {
        for(const auto& channel : m_film_channels)
        {
            channel.second->WriteImage(directory);
        }
    }
}