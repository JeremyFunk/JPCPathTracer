#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include <string>
#include <OpenImageIO/imageio.h>
#include <algorithm>
#include <vector>

namespace jpctracer::utility {

struct Image
{
    Image(uint _width,uint _height);
    std::vector<Vec3> rgbs;
    Vec3& operator[](UInt2 coords);
    Vec3 operator[](UInt2 coords) const;
    
    uint width;
    uint height;
    
};

void WriteImage(std::string path,const unsigned char* pixels, const int width,
                const int height);

void WriteImage(std::string path,const Image& image);

Image ReadImage(std::string path);
}

