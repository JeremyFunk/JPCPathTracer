
#include "gtest/gtest.h"
#include <vector>
#include "utilities/ImageIO.h"
namespace jpc_tracer {

    TEST(utilities,write_image)
    {
        std::vector<int> image = {
            0,255,255, 255,255,0,
            255, 0, 255, 255, 255, 255
        };

        std::vector<unsigned char> char_img;

        for(auto val: image)
            char_img.push_back(val);
        
        WriteImage("foo.png",char_img.data(),2,2);
        
    }


}