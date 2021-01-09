#pragma once
#include "OpenImageIO/imageio.h"
#include "textures/Texture.h"
#include "color/ColorValue.h"
#include <string>
#include <iostream>

namespace utilities
{
    class textureLoader
    {
    public:
        static std::shared_ptr<textures::Texture> LoadTexture(std::string path){
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
                
                return std::make_shared<textures::Texture>(std::make_shared<std::vector<double>>(pixels), xres, yres);
            }

            return std::make_shared<textures::Texture>(nullptr,0,0);
        }

        
        static std::shared_ptr<color::ColorValueTexture> LoadColorValueTexture(std::string path){

            return std::make_shared<color::ColorValueTexture>(LoadTexture(path));
        }
    };
}