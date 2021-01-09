#include "ImageIO.h"
namespace utilities
{
    void WriteImage(std::string path,const unsigned char* pixels, const int width,
                    const int height) 
    {
        const char *filename = path.data();
        const int channels = 3; // RGB

        std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(filename);
        if (!out)
            return;
        OIIO::ImageSpec spec(width, height, channels, OIIO::TypeDesc::UINT8);
        out->open(filename, spec);
        out->write_image(OIIO::TypeDesc::UINT8, pixels);
        out->close();
    }
}