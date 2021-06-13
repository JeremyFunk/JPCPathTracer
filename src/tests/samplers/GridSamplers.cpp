#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/samplers/samplers.h"
#include "gtest/gtest.h"
#include <iterator>
#include <vector>

namespace jpctracer
{
TEST(samplers, DebugSamplerTest)
{
    auto sampler = sampler::DebugSampler();
    std::vector<Vec2> vec(5 * 5);
    sampler.Sample({5, 5}, &vec[0]);
    for (const auto& v : vec)
        std::cout << v[0] << "," << v[1] << "|";
    std::cout << "\n";
}
TEST(samplers, StratifiedSampler)
{
    auto sampler = sampler::StratifiedSampler();
    int samples_x = 20;
    int samples_y = 20;
    std::vector<Vec2> samples(samples_x * samples_y);
    sampler.Sample({20, 20}, samples.data());
}
} // namespace jpctracer