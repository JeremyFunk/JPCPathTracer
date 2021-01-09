#include "DebugSampler.h"
#include "core/Linalg.h"

namespace samplers
{
    core::Prec DebugSampler::Get1DSample() 
    {
        return 0.5;
    }
    
    core::Vec2 DebugSampler::Get2DSample() 
    {
        return {Get1DSample(), Get1DSample()};
    }
    
    std::shared_ptr<core::ISampler> DebugSampler::Clone() 
    {
        return std::make_shared<DebugSampler>();
    }
    
    std::unique_ptr<std::vector<core::Prec>> DebugSampler::Get1DSampleArray(size_t size)
    {
        return nullptr;
    }
    
    std::unique_ptr < std::vector < core::Vec2 >> DebugSampler::Get2DSampleArray(size_t size_x, size_t size_y)
    {
        
        return nullptr;
    }
}