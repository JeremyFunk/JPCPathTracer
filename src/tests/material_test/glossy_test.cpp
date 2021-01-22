#include "gtest/gtest.h"
#include <memory>
#include "materials/GlossyMaterial.h"
#include "color/ColorValue.h"
TEST(glossy, test1)
{
    auto color_red = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({1,0.2,0.2}));
    auto mat = std::make_shared<materials::GlossyMaterial>(color_red,0.1);
}