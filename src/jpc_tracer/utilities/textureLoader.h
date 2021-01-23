#pragma once
#include "OpenImageIO/imageio.h"
#include "textures/Texture.h"
#include "color/ColorValue.h"
#include <string>
#include <iostream>

namespace jpc_tracer
{
    class textureLoader
    {
    public:
        static Ref<Texture> LoadTexture(std::string path){
            auto in = OIIO::ImageInput::open(path);
            if(!in){
                std::cout<<"Image File "<<path<<" could not be opened!";
            }else{
                const OIIO::ImageSpec &spec = in->spec();
                int xres = spec.width;
                int yres = spec.height;
                
                int channels = spec.nchannels;
                std::vector<double> pixels (xres*yres*channels);
                in->read_image(OIIO::TypeDesc::DOUBLE, &pixels[0]);
                in->close();
                
                return MakeRef<Texture>(MakeRef<std::vector<double>>(pixels), xres, yres);
            }

            return MakeRef<Texture>(nullptr,0,0);
        }

        
        static Ref<ColorValueTexture> LoadColorValueTexture(std::string path){

            return MakeRef<ColorValueTexture>(LoadTexture(path));
        }
    };
}