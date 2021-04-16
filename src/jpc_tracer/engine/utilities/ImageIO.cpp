#include "ImageIO.h"
#include <iostream>
#include "jpc_tracer/core/core.h"
namespace jpctracer::utility
{
    Image::Image(uint _width,uint _height) 
        : rgbs(_width*_height,{1,0,1}),width(_width),height(_height)
    {
        
    }

    Vec3 Image::operator[](UInt2 coords) const
    {
        return rgbs[coords[1]*width+coords[0]];
    }

    Vec3& Image::operator[](UInt2 coords) 
    {
        return rgbs[coords[1]*width+coords[0]];
    }

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
        JPC_LOG_INFO("Wrote Image");
    }
    
    void WriteImage(std::string path,const Image& image) 
    {
        std::vector<unsigned char> pixels(image.width*image.height*3);
        for(uint y = 0; y < image.height; y++)
        {
            for(uint x = 0;x<image.width;x++)
            {
                auto[r,g,b] = image[{x,y}].Data;
                pixels[3*(y*image.width+x)] = r*255;
                pixels[3*(y*image.width+x)+1] = g*255;
                pixels[3*(y*image.width+x)+2] = b*255;
            }
        }
        WriteImage(path,&pixels[0],image.width,image.height);
    }

    Image ReadImage(std::string path)
    {
        auto in = OIIO::ImageInput::open (path);
        if (! in)
        {
            JPC_LOG_ERROR("Could not open image {}",path);
            return Image(10,10);
        }
        const OIIO::ImageSpec &spec = in->spec();
        int xres = spec.width;
        int yres = spec.height;
        int channels = spec.nchannels;
        if(channels>3)
        {
            JPC_LOG_INFO("Image {} has more then three channels",path);
        }
        else if(channels<3)
        {
            JPC_LOG_ERROR("Image {} has less then three channels",path);
            return Image(10,10);
        }
        
        std::vector<unsigned char> pixels (xres*yres*channels);
        in->read_image (OIIO::TypeDesc::UINT8, &pixels[0]);
        in->close ();

        Image image(xres,yres);
        for(uint y = 0; y < yres; y++)
        {
            for(uint x = 0; x < xres; x++)
            {
                image[{x,y}] = {
                    (Prec)pixels[channels*(y*xres+x)] / 255.f,
                    (Prec)pixels[channels*(y*xres+x)+1] / 255.f,
                    (Prec)pixels[channels*(y*xres+x)+2] / 255.f
                };
                auto result = image[{x,y}].Data;
            }
        }
        return image;
    }
}