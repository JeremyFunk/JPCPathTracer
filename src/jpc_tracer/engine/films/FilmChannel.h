#pragma once
#include "jpc_tracer/core/maths/Spectrum.h"
#include <vector>
#include <string>
namespace jpctracer
{
    class FilmChannel
    {
    public:
        FilmChannel(std::string name, int width,int height);

        void SavePixel(UInt2 pixel, Spectrum spec);
        void WriteImage(std::string directory) const;

        int GetWidth() {return m_width;};
        int GetHeight() {return m_height;};

    private:
        std::vector<unsigned char> m_pixels = {0};
        int m_width;
        int m_height;
        std::string m_name;

    };
}