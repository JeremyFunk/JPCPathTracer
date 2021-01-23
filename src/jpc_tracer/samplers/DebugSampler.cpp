#include "DebugSampler.h"
#include "core/Linalg.h"
#include <memory>

namespace jpc_tracer
{
    Prec DebugSampler::Get1DSample() 
    {
        return 0.5;
    }
    
    Vec2 DebugSampler::Get2DSample() 
    {
        return {Get1DSample(), Get1DSample()};
    }
    
    Ref<ISampler> DebugSampler::Clone() 
    {
        return MakeRef<DebugSampler>();
    }
    
    void DebugSampler::Get1DSampleArray(size_t dim, size_t sample_count, Prec* desination) 
    {
        for(int i = 0; i <dim*sample_count; i++ ) 
            desination[i] = Get1DSample();
    }
    
    void DebugSampler::Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, Vec2* desination) 
    {
        for(int i = 0; i <dim_y*dim_x*sample_count; i++ ) 
            desination[i] = Get2DSample();
    }
    

}