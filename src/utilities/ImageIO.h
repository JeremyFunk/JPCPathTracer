#pragma once
#include <string>
#include <OpenImageIO/imageio.h>
#include <algorithm>

namespace jpc_tracer {

void WriteImage(std::string path,const unsigned char* pixels, const int width,
                const int height);
}

