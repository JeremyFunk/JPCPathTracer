#include "gtest/gtest.h"
#include <iterator>
#include <vector>
#include "jpc_tracer/samplers/GridSampler.h"
#include "jpc_tracer/samplers/samplers.h"

namespace jpctracer {
    TEST(samplers,DebugSamplerTest)
    {
        auto sampler = Build(DebugSamplerBuilder());
        std::vector<Vec2> vec;
        auto insert_it = std::insert_iterator(vec,vec.begin());
        sampler({5,5},insert_it);
        for(const auto& v : vec) std::cout<<v[0]<<","<<v[1]<<"|";
        std::cout<<"\n";

        
    }

}