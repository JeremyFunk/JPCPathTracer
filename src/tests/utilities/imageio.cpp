#include "gtest/gtest.h"
#include "jpc_tracer/engine/utilities/ImageIO.h"
#include "../test_utils.h"


namespace jpctracer {
    TEST(utilities_test,image)
    {
        utility::Image expected_img(10,20);
        for(uint y = 0; y< expected_img.height;y++)
            for(uint x = 0; x < expected_img.width;x++)
                expected_img[{x,y}] = {(Prec)(x)/expected_img.width,(Prec)(y)/expected_img.height,1.f};
        std::string path = "test_img.png";
        utility::WriteImage(path,expected_img);
        auto actual_img = utility::ReadImage(path);
        for(uint y = 0; y< expected_img.height;y++)
            for(uint x = 0; x < expected_img.width;x++)
            {
                TestVec3(expected_img[{x,y}], actual_img[{x,y}],0.1);
            }
    }
}