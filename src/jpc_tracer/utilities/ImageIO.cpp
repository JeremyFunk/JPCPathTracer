#include "ImageIO.h"
#include <iostream>
namespace jpc_tracer
{
    void WriteImage(std::string path,const unsigned char* pixels, const int width,
                    const int height) 
    {
        const char *filename = path.data();
        const int channels = 3; // RGB

        std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(filename);
        if (!out)
        {
            std::cout<<"Could not create image"<<std::endl;
            return;
        }
        OIIO::ImageSpec spec(width, height, channels, OIIO::TypeDesc::UINT8);
        out->open(filename, spec);
        out->write_image(OIIO::TypeDesc::UINT8, pixels);
        out->close();
        std::cout<<"Wrote image"<<std::endl;
    }
}