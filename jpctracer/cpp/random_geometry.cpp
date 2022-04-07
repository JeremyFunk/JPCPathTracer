#include "random_geometry.hpp"
#include <random>

namespace jpc
{
static std::random_device g_rnd_device;
static std::uint32_t      g_seed = g_rnd_device();
void                      set_seed(std::uint32_t seed)
{
    g_seed = seed;
}

std::uint32_t get_seed()
{
    return g_seed;
}

static std::mt19937 engine{g_seed};

std::mt19937& get_random_engine()
{
    static std::uint32_t old_seed = 0;
    if (old_seed != g_seed)
    {
        engine = std::mt19937{g_seed};
        old_seed = g_seed;
    }
    return engine;
}

} // namespace jpc
