#include "gtest/gtest.h"
#include <memory>
#include "materials/GlossyMaterial.h"
#include "color/ColorValue.h"
namespace jpc_tracer {

TEST(glossy, test1)
{
    auto color_red = MakeRef<ColorValueVec3>(ColorValueVec3({1,0.2,0.2}));
    auto mat = MakeRef<GlossyMaterial>(color_red,0.1);
}

}