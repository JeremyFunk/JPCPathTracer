#include "DebugSampler.h"
#include "core/Linalg.h"
#include <memory>

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
    
    void DebugSampler::Get1DSampleArray(size_t dim, size_t sample_count, core::Prec* desination) 
    {
        for(int i = 0; i <dim*sample_count; i++ ) 
            desination[i] = Get1DSample();
    }
    
    void DebugSampler::Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, core::Vec2* desination) 
    {
        for(int i = 0; i <dim_y*dim_x*sample_count; i++ ) 
            desination[i] = Get2DSample();
    }
    

}